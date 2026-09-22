use std::collections::btree_map;
use std::collections::BTreeMap;
use std::fmt;
use std::iter::FusedIterator;

use crate::Element;

/// A sparse vector: a mapping from integer index to arbitrary value, in which
/// only the positions that differ from a default value are stored.
///
/// Because only non-default positions are kept, a vector occupies O(k) memory for
/// k stored entries no matter how far apart the indices are, and whether they are
/// large or negative. Indices need not be contiguous, and an index holding no
/// explicit entry simply reports the default value.
pub struct SparseVector<T = f64> {
    // Ordered on purpose: `keys`, `values`, `elements` and `iter` all guarantee
    // ascending index order, which this container gives for free.
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

    /// The value reported for every position that has no explicit entry.
    ///
    /// There is deliberately no `&mut` counterpart: mutating the default has to
    /// prune, which [`set_default_value`](Self::set_default_value) does.
    pub fn default_value(&self) -> &T {
        &self.default
    }

    /// The number of explicitly stored entries.
    pub fn len(&self) -> usize {
        self.entries.len()
    }

    /// Whether no entry is stored at all.
    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    /// Whether `index` has an explicitly stored entry.
    pub fn contains_key(&self, index: i64) -> bool {
        self.entries.contains_key(&index)
    }

    /// Removes every explicit entry, keeping the default value.
    pub fn clear(&mut self) {
        self.entries.clear();
    }

    /// The indices holding an explicit entry, in ascending order.
    pub fn keys(&self) -> Vec<i64> {
        self.entries.keys().copied().collect()
    }

    /// The index of the entry at 0-based ordinal `n` of [`elements`](Self::elements),
    /// counting from the left.
    ///
    /// The ordinal numbers the entries, not the positions: ordinal `0` is the
    /// leftmost stored entry however far out its index lies.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn element_index(&self, n: usize) -> i64 {
        *self.at_ordinal(n, false).0
    }

    /// The index of the entry at 0-based ordinal `n` of [`elements`](Self::elements),
    /// counting from the right.
    ///
    /// Ordinal `0` is the rightmost stored entry.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn last_element_index(&self, n: usize) -> i64 {
        *self.at_ordinal(n, true).0
    }

    /// Iterates the explicit entries in ascending index order, borrowing them.
    ///
    /// Reach for this instead of [`elements`](Self::elements) when you do not
    /// want to allocate.
    pub fn iter(&self) -> Iter<'_, T> {
        Iter {
            inner: self.entries.iter(),
        }
    }

    /// The entry at 0-based ordinal `n` from whichever end.
    ///
    /// Walks the map rather than going through [`elements`](Self::elements),
    /// which would copy the whole array to read one element of it.
    fn at_ordinal(&self, n: usize, from_right: bool) -> (&i64, &T) {
        let entry = if from_right {
            self.entries.iter().nth_back(n)
        } else {
            self.entries.iter().nth(n)
        };
        match entry {
            Some(entry) => entry,
            None => {
                let side = if from_right { "right" } else { "left" };
                panic!(
                    "SparseVector holds {} entries, so there is no entry {} from the {}",
                    self.entries.len(),
                    n,
                    side
                )
            }
        }
    }

    /// The outermost stored index, the first one or the last one.
    ///
    /// # Panics
    ///
    /// Panics when nothing is stored, there being no position to measure from.
    fn anchor(&self, from_left: bool) -> i64 {
        let index = if from_left {
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
    /// Creates a vector whose empty positions read back as `0.0`.
    ///
    /// This exists only for `f64`, where the `0` the shared semantics name is a
    /// value of the right type; for any other `T` use
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
    /// The value at `index`, or the default value when nothing is stored there.
    ///
    /// Reading is total: every `i64`, negative, huge or far outside anything ever
    /// written, returns a value rather than failing, so this returns `T` and not
    /// `Option<T>`.
    pub fn get(&self, index: i64) -> T {
        match self.entries.get(&index) {
            Some(value) => value.clone(),
            None => self.default.clone(),
        }
    }

    /// The explicit entries, in ascending index order.
    ///
    /// This is the serialization shape described on [`Element`].
    pub fn elements(&self) -> Vec<Element<T>> {
        self.entries
            .iter()
            .map(|(&index, value)| Element {
                index,
                value: value.clone(),
            })
            .collect()
    }

    /// The stored values, in ascending index order.
    pub fn values(&self) -> Vec<T> {
        self.entries.values().cloned().collect()
    }

    /// The element at 0-based ordinal `n` of [`elements`](Self::elements),
    /// counting from the left.
    ///
    /// The ordinal numbers the entries, not the positions: `element(0)` is the
    /// leftmost stored entry however far out its index lies.
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

    /// The value of the entry at 0-based ordinal `n`, counting from the left.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn element_value(&self, n: usize) -> T {
        self.at_ordinal(n, false).1.clone()
    }

    /// The element at 0-based ordinal `n` of [`elements`](Self::elements),
    /// counting from the right.
    ///
    /// `last_element(0)` is the rightmost stored entry.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn last_element(&self, n: usize) -> Element<T> {
        let (&index, value) = self.at_ordinal(n, true);
        Element {
            index,
            value: value.clone(),
        }
    }

    /// The value of the entry at 0-based ordinal `n`, counting from the right.
    ///
    /// # Panics
    ///
    /// Panics when fewer than n + 1 entries are stored.
    pub fn last_element_value(&self, n: usize) -> T {
        self.at_ordinal(n, true).1.clone()
    }

    /// Treating the first stored entry as the first significant digit, the value
    /// `n` positions to its right.
    ///
    /// Positions without an entry in between count, the way the zeros inside a
    /// number count towards its sign. `n` may be negative, which walks left of
    /// that first entry instead; the result is then the default value.
    ///
    /// # Panics
    ///
    /// Panics when nothing is stored, so there is no position to measure from, or
    /// when the position leaves the `i64` range.
    pub fn left_significant_value(&self, n: i64) -> T {
        let anchor = self.anchor(true);
        let index = anchor
            .checked_add(n)
            .unwrap_or_else(|| panic!("SparseVector position {anchor} + {n} overflows i64"));
        self.get(index)
    }

    /// Treating the last stored entry as the first significant digit counting
    /// from the right, the value `n` positions to its left.
    ///
    /// `n` may be negative, which walks right of that last entry instead.
    ///
    /// # Panics
    ///
    /// Panics when nothing is stored, so there is no position to measure from, or
    /// when the position leaves the `i64` range.
    pub fn right_significant_value(&self, n: i64) -> T {
        let anchor = self.anchor(false);
        let index = anchor
            .checked_sub(n)
            .unwrap_or_else(|| panic!("SparseVector position {anchor} - {n} overflows i64"));
        self.get(index)
    }
}

impl<T: PartialEq> SparseVector<T> {
    /// Inserts or updates `value` at `index`, returning `&mut self` so calls chain.
    ///
    /// Writing a value equal to the default removes whatever was there instead of
    /// storing it: that is what keeps the structure sparse.
    pub fn set(&mut self, index: i64, value: T) -> &mut Self {
        if value == self.default {
            self.entries.remove(&index);
        } else {
            self.entries.insert(index, value);
        }
        self
    }

    /// Replaces the default value, immediately dropping every entry equal to it.
    ///
    /// Pruning is one-way: the previous default is not remembered, so positions
    /// dropped here never come back.
    pub fn set_default_value(&mut self, next: T) {
        self.default = next;
        self.prune();
    }

    /// Removes the explicit entry at `index` and returns it, if there was one.
    ///
    /// The position subsequently reads back as the default value. Returned as an
    /// `Option` rather than a `bool` so the removed value is available;
    /// `.is_some()` recovers the boolean.
    pub fn remove(&mut self, index: i64) -> Option<T> {
        self.entries.remove(&index)
    }

    /// Builds a vector from a sequence of elements.
    ///
    /// Entries equal to the default are dropped, and for a repeated index the
    /// last element wins, both because this goes through [`set`](Self::set).
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

    /// Drops every entry equal to the current default.
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

/// An iterator over the entries of a [`SparseVector`], in ascending index order.
///
/// Yields [`Element<&T>`](Element), borrowing the stored values.
pub struct Iter<'a, T> {
    inner: btree_map::Iter<'a, i64, T>,
}

impl<'a, T> Iterator for Iter<'a, T> {
    type Item = Element<&'a T>;

    fn next(&mut self) -> Option<Self::Item> {
        self.inner
            .next()
            .map(|(&index, value)| Element { index, value })
    }

    fn size_hint(&self) -> (usize, Option<usize>) {
        self.inner.size_hint()
    }
}

impl<'a, T> DoubleEndedIterator for Iter<'a, T> {
    fn next_back(&mut self) -> Option<Self::Item> {
        self.inner
            .next_back()
            .map(|(&index, value)| Element { index, value })
    }
}

impl<T> ExactSizeIterator for Iter<'_, T> {
    fn len(&self) -> usize {
        self.inner.len()
    }
}

impl<T> FusedIterator for Iter<'_, T> {}
