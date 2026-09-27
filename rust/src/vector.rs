use std::collections::btree_map;
use std::collections::BTreeMap;
use std::fmt;
use std::iter::FusedIterator;

use crate::Element;

/// A sparse vector: a mapping from integer index to arbitrary value that stores only the
/// positions differing from the default value. Entries are ordered largest index leftmost.
pub struct SparseVector<T = f64> {
    // Kept ascending, the order every ordered view reverses.
    entries: BTreeMap<i64, T>,
    default: T,
}

impl<T> SparseVector<T> {
    /// Creates a vector whose empty positions read back as `default`.
    pub fn with_default(default: T) -> Self {
        Self {
            entries: BTreeMap::new(),
            default,
        }
    }

    /// The value reported for every position with no explicit entry.
    ///
    /// There is deliberately no `&mut` counterpart; use
    /// [`set_default_value`](Self::set_default_value), which prunes.
    pub fn get_default_value(&self) -> &T {
        &self.default
    }

    /// The number of explicitly stored entries.
    pub fn get_element_amount(&self) -> usize {
        self.entries.len()
    }

    /// The distance between the leftmost and rightmost entries, both included; zero when empty.
    ///
    /// # Panics
    ///
    /// Panics when the span leaves the `i64` range.
    pub fn get_significant_dimension(&self) -> i64 {
        match (self.entries.first_key_value(), self.entries.last_key_value()) {
            (Some((&smallest, _)), Some((&largest, _))) => largest
                .checked_sub(smallest)
                .and_then(|span| span.checked_add(1))
                .unwrap_or_else(|| panic!("SparseVector span {smallest}..={largest} overflows i64")),
            _ => 0,
        }
    }

    /// How far the vector reaches above zero: the leftmost index itself, or zero.
    pub fn get_plus_dimension(&self) -> i64 {
        match self.entries.last_key_value() {
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
        match self.entries.first_key_value() {
            Some((&rightmost, _)) if rightmost < 0 => rightmost
                .checked_neg()
                .unwrap_or_else(|| panic!("SparseVector index {rightmost} cannot be negated")),
            _ => 0,
        }
    }

    /// Resets every explicit entry, keeping the default value.
    pub fn reset_vector(&mut self) {
        self.entries.clear();
    }

    /// The stored indices, descending.
    pub fn indexes(&self) -> Vec<i64> {
        self.entries.keys().rev().copied().collect()
    }

    /// The stored indices, ascending.
    pub fn inverted_indexes(&self) -> Vec<i64> {
        self.entries.keys().copied().collect()
    }

    /// The index of the (n+1)-th entry from the left; the ordinal numbers the entries, not
    /// the positions.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn element_index(&self, n: usize) -> i64 {
        *self.at_ordinal(n, false).0
    }

    /// The index of the (n+1)-th entry from the right.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn inverted_element_index(&self, n: usize) -> i64 {
        *self.at_ordinal(n, true).0
    }

    /// Iterates the entries descending by index, borrowing them, without allocating.
    pub fn iter(&self) -> Iter<'_, T> {
        Iter {
            inner: self.entries.iter(),
        }
    }

    /// The entry at 0-based ordinal `n` from whichever end.
    fn at_ordinal(&self, n: usize, inverted: bool) -> (&i64, &T) {
        let entry = if inverted {
            self.entries.iter().nth(n)
        } else {
            self.entries.iter().nth_back(n)
        };
        match entry {
            Some(entry) => entry,
            None => {
                let side = if inverted { "right" } else { "left" };
                panic!(
                    "SparseVector holds {} entries, so there is no entry {} from the {}",
                    self.entries.len(),
                    n,
                    side
                )
            }
        }
    }

    fn anchor(&self, inverted: bool) -> i64 {
        let index = if inverted {
            self.entries.keys().next()
        } else {
            self.entries.keys().next_back()
        };
        match index {
            Some(&index) => index,
            None => panic!("SparseVector holds no entries, so there is nothing to measure from"),
        }
    }
}

impl SparseVector<f64> {
    /// Creates a vector whose empty positions read back as `0.0`; for any other `T` use
    /// [`with_default`](Self::with_default).
    pub fn new() -> Self {
        Self::with_default(0.0)
    }
}

impl<T: Default> Default for SparseVector<T> {
    fn default() -> Self {
        Self::with_default(T::default())
    }
}

impl<T: Clone> SparseVector<T> {
    /// The value at `index`, or the default value when nothing is stored there. Reading is
    /// total, so it returns `T` and not `Option<T>`.
    pub fn get(&self, index: i64) -> T {
        match self.entries.get(&index) {
            Some(value) => value.clone(),
            None => self.default.clone(),
        }
    }

    /// The explicit entries, descending by index: the serialization shape described on
    /// [`Element`].
    pub fn elements(&self) -> Vec<Element<T>> {
        self.entries
            .iter()
            .rev()
            .map(|(&index, value)| Element {
                index,
                value: value.clone(),
            })
            .collect()
    }

    /// The explicit entries, ascending by index.
    pub fn inverted_elements(&self) -> Vec<Element<T>> {
        self.entries
            .iter()
            .map(|(&index, value)| Element {
                index,
                value: value.clone(),
            })
            .collect()
    }

    /// The stored values, descending by index.
    pub fn values(&self) -> Vec<T> {
        self.entries.values().rev().cloned().collect()
    }

    /// The stored values, ascending by index.
    pub fn inverted_values(&self) -> Vec<T> {
        self.entries.values().cloned().collect()
    }

    /// The (n+1)-th entry from the left; the ordinal numbers the entries, not the positions.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn element(&self, n: usize) -> Element<T> {
        let (&index, value) = self.at_ordinal(n, false);
        Element {
            index,
            value: value.clone(),
        }
    }

    /// The value of the (n+1)-th entry from the left.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn element_value(&self, n: usize) -> T {
        self.at_ordinal(n, false).1.clone()
    }

    /// The (n+1)-th entry from the right, `inverted_element(0)` being the rightmost.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn inverted_element(&self, n: usize) -> Element<T> {
        let (&index, value) = self.at_ordinal(n, true);
        Element {
            index,
            value: value.clone(),
        }
    }

    /// The value of the (n+1)-th entry from the right.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn inverted_element_value(&self, n: usize) -> T {
        self.at_ordinal(n, true).1.clone()
    }

    /// The value `n` positions right of the leftmost entry, empty positions counting; a
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

    /// The value `n` positions left of the rightmost entry; a negative `n` walks right of it.
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
    /// Inserts or updates the entry at `index`, returning `&mut self` so calls chain;
    /// writing the default removes instead.
    pub fn set(&mut self, index: i64, value: T) -> &mut Self {
        if value == self.default {
            self.entries.remove(&index);
        } else {
            self.entries.insert(index, value);
        }
        self
    }

    /// Replaces the default value, immediately dropping every entry equal to it. Pruning is
    /// one-way: dropped positions never come back.
    pub fn set_default_value(&mut self, next: T) {
        self.default = next;
        self.prune();
    }

    /// Resets the entry at `index` and returns it if there was one. Returned as an `Option`
    /// rather than a `bool`, so `.is_some()` recovers the boolean.
    pub fn reset_value(&mut self, index: i64) -> Option<T> {
        self.entries.remove(&index)
    }

    /// Builds from any iterator of elements, dropping those equal to the default and keeping
    /// the last of a repeated index.
    pub fn from_elements<I>(elements: I, default: T) -> Self
    where
        I: IntoIterator<Item = Element<T>>,
    {
        let mut vector = Self::with_default(default);
        for element in elements {
            vector.set(element.index, element.value);
        }
        vector
    }

    fn prune(&mut self) {
        let default = &self.default;
        self.entries.retain(|_, value| value != default);
    }
}

impl<T: Clone> Clone for SparseVector<T> {
    /// An independent copy, default value included.
    fn clone(&self) -> Self {
        Self {
            entries: self.entries.clone(),
            default: self.default.clone(),
        }
    }
}

// The storage as it is, ascending, not one of the ordered views.
impl<T: fmt::Debug> fmt::Debug for SparseVector<T> {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        f.debug_struct("SparseVector")
            .field("default", &self.default)
            .field("entries", &self.entries)
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

/// An iterator over the entries of a [`SparseVector`], descending by index and borrowing the
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
