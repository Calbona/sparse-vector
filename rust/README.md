**English** · [Deutsch](https://github.com/Calbona/sparse-vector/blob/main/docs/de-DE.md) · [Español](https://github.com/Calbona/sparse-vector/blob/main/docs/es-ES.md) · [Français](https://github.com/Calbona/sparse-vector/blob/main/docs/fr-FR.md) · [Italiano](https://github.com/Calbona/sparse-vector/blob/main/docs/it-IT.md) · [日本語](https://github.com/Calbona/sparse-vector/blob/main/docs/ja-JP.md) · [한국어](https://github.com/Calbona/sparse-vector/blob/main/docs/ko-KR.md) · [Русский](https://github.com/Calbona/sparse-vector/blob/main/docs/ru-RU.md) · [Tiếng Việt](https://github.com/Calbona/sparse-vector/blob/main/docs/vi-VN.md) · [简体中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-CN.md) · [繁體中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-TW.md)

# sparse-vector-rs

Rust implementation of [sparse-vector](https://github.com/Calbona/sparse-vector#readme) — a mapping from integer index to arbitrary value that stores only the positions differing from a default value

## Installation

```sh
cargo add sparse-vector-rs
```

## Quick start

```rust
use sparse_vector::SparseVector;

let mut vector = SparseVector::new(""); // empty positions read back as ""

vector.set(1_000_000, "far away");
vector.set(-42, "negative");

assert_eq!(vector.get(1_000_000), "far away");
assert_eq!(vector.get(-42), "negative");
assert_eq!(vector.get(7), ""); // no element there, so the default value
assert_eq!(vector.get_element_amount(), 2);
```

`SparseVector::default_new()` is the same with the default left out, which is then `T::default()` — `0.0` for `f64`:

```rust
use sparse_vector::SparseVector;

let vector: SparseVector<f64> = SparseVector::default_new();
assert_eq!(vector.get_default_value(), &0.0);
assert_eq!(vector.get(1_000_000), 0.0);
```

The default value is replaceable, and replacing it prunes immediately:

```rust
use sparse_vector::SparseVector;

let mut vector = SparseVector::new(0);
vector.set(1, 7);
vector.set(2, 9);

// Position 1 held 7, so it is dropped; position 2 keeps its value.
assert!(vector.set_default_value(7));
assert_eq!(vector.elements().len(), 1);
```

The type of the values is not restricted — anything with `Clone` can be read back, anything with `PartialEq` can be stored:

```rust
use sparse_vector::{Element, SparseVector};

let mut vector: SparseVector<Option<i32>> = SparseVector::new(None);
vector.set(3, Some(1));

assert_eq!(vector.get(3), Some(1));
assert_eq!(vector.get(4), None); // empty positions are None
```

A predicate replaces `PartialEq` wherever the default value is compared, pruning included:

```rust
use std::sync::Arc;

use sparse_vector::SparseVector;

let mut vector = SparseVector::new("ab".to_string());
vector.set_equality(Some(Arc::new(|value: &String, default: &String| {
    value.len() == default.len()
})));

vector.set(1, "xy".to_string()); // a different string of the same length
assert_eq!(vector.get_element_amount(), 0);
```

Two vectors are compared as a whole by `is_equal_to`, and what differs is listed by `differences`. Both decide every comparison with the **receiver's** predicate:

```rust
use sparse_vector::{Element, SparseVector};

let mut a = SparseVector::new(0);
a.set(1, 7);
let mut b = SparseVector::new(0);
b.set(1, 7);
b.set(2, 9);

assert!(!a.is_equal_to(&b)); // position 2 differs
assert_eq!(a.differences(&b), [Element { index: 2, value: 0 }]);
```

## API

| Member | Description |
| --- | --- |
| `new(default: T) -> SparseVector<T>` | Builds a sparse vector |
| `default_new() -> SparseVector<T>` | Builds a sparse vector with the default left out, which is then `T::default()`. Needs `T: Default` |
| `get_default_value(&self) -> &T` | The value reported for positions with no explicit element |
| `set_default_value(&mut self, next: T) -> bool` | Replaces the default, immediately pruning every element equal to it, and returns whether anything was pruned |
| `get_equality(&self) -> Option<&Equality<T>>` | The predicate replacing `PartialEq`, or `None` when `PartialEq` decides |
| `set_equality(&mut self, equality: Option<Equality<T>>) -> bool` | Replaces the predicate, immediately pruning every element it calls equal to the default, and returns whether anything was pruned. `None` restores `PartialEq` |
| `get_element_amount(&self) -> usize` | Number of explicitly stored elements |
| `get_significant_dimension(&self) -> i64` | The distance between the leftmost and rightmost elements, both included — `-2` and `5` are `8`. `0` when nothing is stored |
| `get_plus_dimension(&self) -> i64` | How far the vector reaches above zero: the leftmost index itself. `0` when nothing is stored above zero |
| `get_minus_dimension(&self) -> i64` | How far the vector reaches below zero: the rightmost index, negated. `0` when nothing is stored below zero |
| `get(&self, index: i64) -> T` | The value at `index`, or the default |
| `set(&mut self, index: i64, value: T) -> &mut Self` | Inserts or updates the element, and returns the vector itself, so calls chain |
| `reset_value(&mut self, index: i64) -> bool` | Resets the element back to the default, returning whether there was one |
| `reset_vector(&mut self) -> bool` | Resets every element, keeping the default, returning whether there was any |
| `elements(&self) -> Vec<Element<T>>` | Every element, in descending index order |
| `inverted_elements(&self) -> Vec<Element<T>>` | The same elements in ascending index order |
| `indexes(&self) -> Vec<i64>` | Every stored index, descending |
| `inverted_indexes(&self) -> Vec<i64>` | The same indices in ascending order |
| `values(&self) -> Vec<T>` | Every stored value, in descending index order |
| `inverted_values(&self) -> Vec<T>` | The same values in ascending index order |
| `element(&self, n: usize) -> Element<T>` | The (n+1)-th element from the left, i.e. `elements()[n]` |
| `element_index(&self, n: usize) -> i64` | The index that element sits at |
| `element_value(&self, n: usize) -> T` | Its value |
| `inverted_element(&self, n: usize) -> Element<T>` | The same counting from the right, i.e. `inverted_elements()[n]`; `inverted_element(0)` is the rightmost element |
| `inverted_element_index(&self, n: usize) -> i64` | The index that element sits at |
| `inverted_element_value(&self, n: usize) -> T` | Its value |
| `left_significant_value(&self, n: i64) -> T` | The value `n` positions right of the first stored element |
| `right_significant_value(&self, n: i64) -> T` | The value `n` positions left of the last stored element |
| `iter(&self) -> Iter<'_, T>` | Borrows the stored elements in descending index order |
| `clone()` | An independent copy, default value and equality predicate included |
| `from_elements(elements, default) -> SparseVector<T>` | Builds from an `IntoIterator<Item = Element<T>>` |
| `is_equal_to(&self, other: &Self) -> bool` | Whether `other` holds the same values as `self`, every comparison made by this vector's predicate. A position holding nothing contributes its own vector's default. Never panics. Needs `T: PartialEq` |
| `differences(&self, other: &Self) -> Vec<Element<T>>` | The elements of `self` that differ from `other`, descending by index, each carrying this vector's value there — the default value itself where nothing is stored. Panics unless the two default values compare equal. Needs `T: Clone + T: PartialEq` |

Getting a value and listing need `T: Clone`; writing, resetting and changing the default need `T: PartialEq`. Comparing two vectors needs `T: PartialEq` for `is_equal_to()` and both bounds for `differences()`

A predicate is consulted *instead of* `PartialEq`, but the bound stays: Rust cannot ask for it only while no predicate is set. The comparison methods draw on that same bound

`index` is an `i64`, so a non-integer index does not compile

Reading a position outside the data range does not throw, it returns the default value: that is the point of this type

The `n` the ordinal methods take is a `usize`, so an ordinal cannot be negative and a negative literal does not compile; an ordinal past the end panics

The significant-value methods take a signed `i64`, so a negative `n` walks the other way, into the default value. They panic when the vector holds no element at all, and also when the position would leave the `i64` range

`&SparseVector<T>` is also `IntoIterator`, so `for element in &vector` works and yields `Element<&T>`

`SparseVector<T>` implements `Debug` when `T` does, and has **no `PartialEq`**: with a predicate a vector carries around, there is no single answer for `==` to give. Whole-vector comparison is the named `is_equal_to()` and `differences()`, both answering under the **receiver's** predicate, so `a.is_equal_to(&b)` and `b.is_equal_to(&a)` may disagree when the two carry different predicates. The predicate governs pruning as well, but the two are separate questions: pruning is about one value against one vector's default, comparison is about two vectors

Both comparison methods trust the predicate rather than correcting it: an asymmetric one makes the two directions disagree, a non-reflexive one makes a vector unequal to itself, and a `NaN` default makes both methods treat a vector as unequal to itself — `differences` panicking outright, since the precondition itself fails. The two also fail differently on an unequal default: `is_equal_to` returns `false`, `differences` panics

## Differences from the TypeScript implementation

The semantics are shared, but not everything carries over. Stated here rather than papered over.

**Indices are `i64`, so a non-integer index does not typecheck.** The TypeScript version takes a `number` and throws a `TypeError` for `1.5`, `NaN` or `Infinity`. Here those are compile errors, which is the better failure. The index domain is also strictly larger: TypeScript stops at `Number.MAX_SAFE_INTEGER` (2^53−1), this goes to the full `i64` range.

**Pruning compares with `PartialEq`, or with your own predicate.** For `f64` that is identical to the TypeScript behaviour, `NaN` caveat included: `NaN != NaN`, so a stored `NaN` survives even when the default is also `NaN`.

For your own types it can differ. TypeScript's `===` compares objects by *reference*; Rust's `PartialEq` is almost always *structural*. Two distinct objects with equal contents are one value there and two values here, so a position holding an equal-but-distinct object is pruned here and kept there. Want reference identity? Hand the vector a predicate that says so — `Rc::ptr_eq` is the whole recipe:

```rust
use std::rc::Rc;
use std::sync::Arc;

use sparse_vector::SparseVector;

let first = Rc::new(1);
let second = Rc::new(1); // equal contents, a different allocation

let mut vector = SparseVector::new(first.clone());
vector.set_equality(Some(Arc::new(|value: &Rc<i32>, default: &Rc<i32>| {
    Rc::ptr_eq(value, default)
})));

vector.set(1, second); // a different pointer, so it is kept
vector.set(2, first);  // the same pointer as the default, so it is dropped
assert_eq!(vector.get_element_amount(), 1);
```

**Nothing is omitted by accident.** `default_new()` uses `T::default()`, not a literal `0`. For `f64` that is `0.0`, exactly the documented rule; for `String` it is `""`, which is the same rule generalised rather than a lie about the type.

**There is no JSON encoder here**, nor in the other two implementations: `elements()` is the contract, and serialization is left to whatever crate you already use rather than taken on as a dependency.

## Development

```sh
cargo test          # unit tests, integration tests and the README examples
cargo build         # build the library
cargo doc --no-deps --open
```

The library itself has no dependencies — not even for the tests

## License

MIT
