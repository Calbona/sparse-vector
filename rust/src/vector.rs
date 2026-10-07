use std::collections::btree_map;
use std::collections::BTreeMap;
use std::fmt;
use std::iter::FusedIterator;
use std::panic::RefUnwindSafe;
use std::sync::Arc;

use crate::Element;

/// The custom equality predicate, called with the value under test first and the current
/// default second. It replaces `PartialEq`, in pruning as everywhere else.
///
/// `RefUnwindSafe` is required so that holding a predicate does not cost a
/// `SparseVector` its own `UnwindSafe`.
pub type Equality<T> = Arc<dyn Fn(&T, &T) -> bool + Send + Sync + RefUnwindSafe>;

/// A sparse vector: a mapping from integer index to arbitrary value that stores only the
/// positions differing from the default value. Entries are ordered largest index leftmost.
pub struct SparseVector<T = f64> {
    // Kept ascending, the order every ordered view reverses.
    elements: BTreeMap<i64, T>,
    default: T,
    // `None` compares with `PartialEq`.
    equality: Option<Equality<T>>,
}

impl<T> SparseVector<T> {
    /// Creates a vector whose empty positions read back as `default`.
    pub fn new(default: T) -> Self {
        Self {
            elements: BTreeMap::new(),
            default,
            equality: None,
        }
    }

    /// The value reported for every position with no explicit element.
    ///
    /// There is deliberately no `&mut` counterpart; use
    /// [`set_default_value`](Self::set_default_value), which prunes.
    pub fn get_default_value(&self) -> &T {
        &self.default
    }

    /// The predicate replacing `PartialEq`, or `None` when `PartialEq` decides.
    pub fn get_equality(&self) -> Option<&Equality<T>> {
        self.equality.as_ref()
    }

    /// The number of explicitly stored elements.
    pub fn get_element_amount(&self) -> usize {
        self.elements.len()
    }

    /// The distance between the leftmost and rightmost elements, both included; zero when empty.
    ///
    /// # Panics
    ///
    /// Panics when the span leaves the `i64` range.
    pub fn get_significant_dimension(&self) -> i64 {
        match (self.elements.first_key_value(), self.elements.last_key_value()) {
            (Some((&smallest, _)), Some((&largest, _))) => largest
                .checked_sub(smallest)
                .and_then(|span| span.checked_add(1))
                .unwrap_or_else(|| panic!("SparseVector span {smallest}..={largest} overflows i64")),
            _ => 0,
        }
    }

    /// How far the vector reaches above zero: the leftmost index itself, or zero.
    pub fn get_plus_dimension(&self) -> i64 {
        match self.elements.last_key_value() {
            Some((&leftmost, _)) if leftmost > 0 => leftmost,
            _ => 0,
        }
    }

    /// How far the vector reaches below zero: the rightmost index negated, or zero.
    ///
    /// # Panics
    ///
    /// Panics when the negation leaves the `i64` range.
    pub fn get_minus_dimension(&self) -> i64 {
        match self.elements.first_key_value() {
            Some((&rightmost, _)) if rightmost < 0 => rightmost
                .checked_neg()
                .unwrap_or_else(|| panic!("SparseVector index {rightmost} cannot be negated")),
            _ => 0,
        }
    }

    /// Resets every explicit element, keeping the default value, and returns whether there was any.
    pub fn reset_vector(&mut self) -> bool {
        if self.elements.is_empty() {
            return false;
        }
        self.elements.clear();
        true
    }

    /// The stored indices, descending.
    pub fn indexes(&self) -> Vec<i64> {
        self.elements.keys().rev().copied().collect()
    }

    /// The stored indices, ascending.
    pub fn inverted_indexes(&self) -> Vec<i64> {
        self.elements.keys().copied().collect()
    }

    /// The index of the (n+1)-th element from the left; the ordinal numbers the elements, not
    /// the positions.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 elements are stored.
    pub fn element_index(&self, n: usize) -> i64 {
        *self.at_ordinal(n, false).0
    }

    /// The index of the (n+1)-th element from the right.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 elements are stored.
    pub fn inverted_element_index(&self, n: usize) -> i64 {
        *self.at_ordinal(n, true).0
    }

    /// Iterates the elements descending by index, borrowing them, without allocating.
    pub fn iter(&self) -> Iter<'_, T> {
        Iter {
            inner: self.elements.iter(),
        }
    }

    /// The element at 0-based ordinal `n` from whichever end.
    fn at_ordinal(&self, n: usize, inverted: bool) -> (&i64, &T) {
        let element = if inverted {
            self.elements.iter().nth(n)
        } else {
            self.elements.iter().nth_back(n)
        };
        match element {
            Some(element) => element,
            None => {
                let side = if inverted { "right" } else { "left" };
                panic!(
                    "SparseVector holds {} elements, so there is no element {} from the {}",
                    self.elements.len(),
                    n,
                    side
                )
            }
        }
    }

    fn anchor(&self, inverted: bool) -> i64 {
        let index = if inverted {
            self.elements.keys().next()
        } else {
            self.elements.keys().next_back()
        };
        match index {
            Some(&index) => index,
            None => panic!("SparseVector holds no elements, so there is nothing to measure from"),
        }
    }

    /// The value at `index`, or the default when nothing is stored there, borrowing rather than
    /// copying and panicking on nothing.
    fn stored_or_default(&self, index: i64) -> &T {
        self.elements.get(&index).unwrap_or(&self.default)
    }
}

impl<T: Default> SparseVector<T> {
    /// Creates a vector whose empty positions read back as `T::default()`, the default left
    /// out. For `f64` that is `0.0`.
    pub fn default_new() -> Self {
        Self::new(T::default())
    }
}

impl<T: Clone> SparseVector<T> {
    /// The value at `index`, or the default value when nothing is stored there. Reading is
    /// total, so it returns `T` and not `Option<T>`.
    pub fn get(&self, index: i64) -> T {
        match self.elements.get(&index) {
            Some(value) => value.clone(),
            None => self.default.clone(),
        }
    }

    /// The explicit elements, descending by index: the serialization shape described on
    /// [`Element`].
    pub fn elements(&self) -> Vec<Element<T>> {
        self.elements
            .iter()
            .rev()
            .map(|(&index, value)| Element {
                index,
                value: value.clone(),
            })
            .collect()
    }

    /// The explicit elements, ascending by index.
    pub fn inverted_elements(&self) -> Vec<Element<T>> {
        self.elements
            .iter()
            .map(|(&index, value)| Element {
                index,
                value: value.clone(),
            })
            .collect()
    }

    /// The stored values, descending by index.
    pub fn values(&self) -> Vec<T> {
        self.elements.values().rev().cloned().collect()
    }

    /// The stored values, ascending by index.
    pub fn inverted_values(&self) -> Vec<T> {
        self.elements.values().cloned().collect()
    }

    /// The (n+1)-th element from the left; the ordinal numbers the elements, not the positions.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 elements are stored.
    pub fn element(&self, n: usize) -> Element<T> {
        let (&index, value) = self.at_ordinal(n, false);
        Element {
            index,
            value: value.clone(),
        }
    }

    /// The value of the (n+1)-th element from the left.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 elements are stored.
    pub fn element_value(&self, n: usize) -> T {
        self.at_ordinal(n, false).1.clone()
    }

    /// The (n+1)-th element from the right, `inverted_element(0)` being the rightmost.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 elements are stored.
    pub fn inverted_element(&self, n: usize) -> Element<T> {
        let (&index, value) = self.at_ordinal(n, true);
        Element {
            index,
            value: value.clone(),
        }
    }

    /// The value of the (n+1)-th element from the right.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 elements are stored.
    pub fn inverted_element_value(&self, n: usize) -> T {
        self.at_ordinal(n, true).1.clone()
    }

    /// The value `n` positions right of the leftmost element, empty positions counting; a
    /// negative `n` walks left of it, into the default value.
    ///
    /// # Panics
    ///
    /// Panics when nothing is stored, or the position leaves the `i64` range.
    pub fn left_significant_value(&self, n: i64) -> T {
        let anchor = self.anchor(false);
        let index = anchor
            .checked_sub(n)
            .unwrap_or_else(|| panic!("SparseVector position {anchor} - {n} overflows i64"));
        self.get(index)
    }

    /// The value `n` positions left of the rightmost element; a negative `n` walks right of it.
    ///
    /// # Panics
    ///
    /// Panics when nothing is stored, or the position leaves the `i64` range.
    pub fn right_significant_value(&self, n: i64) -> T {
        let anchor = self.anchor(true);
        let index = anchor
            .checked_add(n)
            .unwrap_or_else(|| panic!("SparseVector position {anchor} + {n} overflows i64"));
        self.get(index)
    }
}

impl<T: PartialEq> SparseVector<T> {
    /// Inserts or updates the element at `index`, returning `&mut self` so calls chain;
    /// writing the default removes instead.
    pub fn set(&mut self, index: i64, value: T) -> &mut Self {
        if self.equals(&value) {
            self.elements.remove(&index);
        } else {
            self.elements.insert(index, value);
        }
        self
    }

    /// Replaces the default value, immediately dropping every element equal to it and returning
    /// whether anything was dropped. Pruning is one-way: dropped positions never come back.
    pub fn set_default_value(&mut self, next: T) -> bool {
        self.default = next;
        self.prune()
    }

    /// Replaces the predicate, immediately dropping every element it calls equal to the default
    /// and returning whether anything was dropped. `None` restores `PartialEq`.
    pub fn set_equality(&mut self, equality: Option<Equality<T>>) -> bool {
        self.equality = equality;
        self.prune()
    }

    /// Resets the element at `index`, returning whether there was one. It then reads as the
    /// default.
    pub fn reset_value(&mut self, index: i64) -> bool {
        self.elements.remove(&index).is_some()
    }

    /// Whether `other` holds the same values as `self`, every comparison made by this vector's
    /// predicate: the two default values first, then each index either vector stores an element
    /// at, a position holding nothing contributing its own vector's default.
    ///
    /// Never panics. Two `NaN` defaults compare unequal, so a vector holding one is not equal to
    /// itself. The three properties of equality hold only as far as the predicate does — an
    /// asymmetric or non-reflexive predicate gives an asymmetric or non-reflexive answer, the
    /// library never correcting it.
    pub fn is_equal_to(&self, other: &Self) -> bool {
        let equality = self.equality.as_ref();
        if !equals_with(equality, other.get_default_value(), self.get_default_value()) {
            return false;
        }
        let mut mine = self.elements.iter().rev().peekable();
        let mut theirs = other.elements.iter().rev().peekable();
        loop {
            let mine_index = mine.peek().map(|element| *element.0);
            let their_index = theirs.peek().map(|element| *element.0);
            let take_mine = match (mine_index, their_index) {
                (None, None) => return true,
                (Some(mine_index), Some(their_index)) => mine_index >= their_index,
                (Some(_), None) => true,
                (None, Some(_)) => false,
            };
            if take_mine {
                let (index, value) = mine.next().expect("peeked above");
                let other_value = if Some(*index) == their_index {
                    theirs.next().expect("peeked above").1
                } else {
                    other.get_default_value()
                };
                if !equals_with(equality, other_value, value) {
                    return false;
                }
            } else {
                let (_, value) = theirs.next().expect("peeked above");
                if !equals_with(equality, value, self.get_default_value()) {
                    return false;
                }
            }
        }
    }

    /// Builds from any iterator of elements, dropping those equal to the default and keeping
    /// the last of a repeated index.
    pub fn from_elements<I>(elements: I, default: T) -> Self
    where
        I: IntoIterator<Item = Element<T>>,
    {
        let mut vector = Self::new(default);
        for element in elements {
            vector.set(element.index, element.value);
        }
        vector
    }

    /// Whether `value` equals the default, by `self.equality` when set and `PartialEq` otherwise.
    fn equals(&self, value: &T) -> bool {
        equals_with(self.equality.as_ref(), value, &self.default)
    }

    /// Drops every element equal to the default, returning whether anything was dropped.
    fn prune(&mut self) -> bool {
        let default = &self.default;
        let equality = self.equality.as_ref();
        let mut pruned = false;
        self.elements.retain(|_, value| {
            let equal = equals_with(equality, value, default);
            pruned |= equal;
            !equal
        });
        pruned
    }
}

// Whole-vector comparison: the one method that reads another vector's elements as well.
impl<T: Clone + PartialEq> SparseVector<T> {
    /// The elements of `self` that differ from `other`, descending by index, each carrying this
    /// vector's value at that index — the default value itself wherever nothing is stored there.
    ///
    /// Not a symmetric difference, and not guaranteed to be empty for `a.differences(a)`: a stored
    /// `NaN` differs from itself, since `NaN != NaN`.
    ///
    /// # Panics
    ///
    /// Panics when the two default values do not compare equal under this vector's predicate, so
    /// there is no sense in which the vectors can be compared.
    pub fn differences(&self, other: &Self) -> Vec<Element<T>> {
        let equality = self.equality.as_ref();
        assert!(
            equals_with(equality, other.get_default_value(), self.get_default_value()),
            "SparseVector default values are not equal, so the two vectors cannot be compared"
        );
        let mut differing = Vec::new();
        let mut mine = self.elements.iter().rev().peekable();
        let mut theirs = other.elements.iter().rev().peekable();
        loop {
            let mine_index = mine.peek().map(|element| *element.0);
            let their_index = theirs.peek().map(|element| *element.0);
            let take_mine = match (mine_index, their_index) {
                (None, None) => break,
                (Some(mine_index), Some(their_index)) => mine_index >= their_index,
                (Some(_), None) => true,
                (None, Some(_)) => false,
            };
            if take_mine {
                let index = *mine.next().expect("peeked above").0;
                let other_value = if Some(index) == their_index {
                    theirs.next().expect("peeked above").1
                } else {
                    other.get_default_value()
                };
                let value = self.stored_or_default(index);
                if !equals_with(equality, other_value, value) {
                    differing.push(Element {
                        index,
                        value: value.clone(),
                    });
                }
            } else {
                let (index, other_value) = theirs.next().expect("peeked above");
                let value = self.stored_or_default(*index);
                if !equals_with(equality, other_value, value) {
                    differing.push(Element {
                        index: *index,
                        value: value.clone(),
                    });
                }
            }
        }
        differing
    }
}

/// Whether `left` equals `right`, by `equality` when set and `PartialEq` otherwise.
///
/// The predicate decides every comparison in this module. Which value goes in the first slot is
/// the caller's choice, and a comparison against another vector always puts its value first.
fn equals_with<T: PartialEq>(equality: Option<&Equality<T>>, left: &T, right: &T) -> bool {
    match equality {
        Some(equality) => equality(left, right),
        None => left == right,
    }
}

impl<T: Clone> Clone for SparseVector<T> {
    /// An independent copy, default value and equality predicate included.
    fn clone(&self) -> Self {
        Self {
            elements: self.elements.clone(),
            default: self.default.clone(),
            equality: self.equality.clone(),
        }
    }
}

// The storage as it is, ascending, not one of the ordered views.
impl<T: fmt::Debug> fmt::Debug for SparseVector<T> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("SparseVector")
            .field("default", &self.default)
            .field("elements", &self.elements)
            .finish()
    }
}

impl<'a, T> IntoIterator for &'a SparseVector<T> {
    type Item = Element<&'a T>;
    type IntoIter = Iter<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

/// An iterator over the elements of a [`SparseVector`], descending by index and borrowing the
/// stored values as [`Element<&T>`](Element).
pub struct Iter<'a, T> {
    inner: btree_map::Iter<'a, i64, T>,
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = Element<&'a T>;

    /// The inner map is ascending while this iterator is descending, so the ends swap here.
    fn next(&mut self) -> Option<Self::Item> {
        self.inner
            .next_back()
            .map(|(&index, value)| Element { index, value })
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        self.inner.size_hint()
    }
}

impl<'a, T> DoubleEndedIterator for Iter<'a, T> {
    fn next_back(&mut self) -> Option<Self::Item> {
        self.inner
            .next()
            .map(|(&index, value)| Element { index, value })
    }
}

impl<T> ExactSizeIterator for Iter<'_, T> {
    fn len(&self) -> usize {
        self.inner.len()
    }
}

impl<T> FusedIterator for Iter<'_, T> {}
