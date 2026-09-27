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

let mut vector = SparseVector::with_default(""); // empty positions read back as ""

vector.set(1_000_000, "far away");
vector.set(-42, "negative");

assert_eq!(vector.get(1_000_000), "far away");
assert_eq!(vector.get(-42), "negative");
assert_eq!(vector.get(7), ""); // no entry there, so the default value
assert_eq!(vector.get_element_amount(), 2);
```

`SparseVector::new()` is the same thing for the default `f64`, starting from `0.0`:

```rust
use sparse_vector::SparseVector;

let vector = SparseVector::new();
assert_eq!(vector.get_default_value(), &0.0);
assert_eq!(vector.get(1_000_000), 0.0);
```

The default value is replaceable, and replacing it prunes immediately:

```rust
use sparse_vector::SparseVector;

let mut vector = SparseVector::with_default(0);
vector.set(1, 7);
vector.set(2, 9);

vector.set_default_value(7);
// Position 1 held 7, so it is dropped; position 2 keeps its value.
assert_eq!(vector.elements().len(), 1);
```

The type of the values is not restricted — anything with `Clone` can be read back, anything with `PartialEq` can be stored:

```rust
use sparse_vector::{Element, SparseVector};

let mut vector: SparseVector<Option<i32>> = SparseVector::with_default(None);
vector.set(3, Some(1));

assert_eq!(vector.get(3), Some(1));
assert_eq!(vector.get(4), None); // empty positions are None
```

## API

| Member | Description |
| --- | --- |
| `new() -> SparseVector<f64>` | Builds a sparse vector, the default left out, which is then `0.0`. Defined on `SparseVector<f64>` only — for another `T`, use `with_default`, or `SparseVector::default()` for `T::default()` |
| `with_default(default: T) -> SparseVector<T>` | Builds a sparse vector |
| `get_default_value(&self) -> &T` | The value reported for positions with no explicit entry |
| `set_default_value(&mut self, next: T)` | Replaces the default, immediately pruning every entry equal to it |
| `get_element_amount(&self) -> usize` | Number of explicitly stored entries |
| `get_significant_dimension(&self) -> i64` | The distance between the leftmost and rightmost entries, both included — `-2` and `5` are `8`. `0` when nothing is stored |
| `get_plus_dimension(&self) -> i64` | How far the vector reaches above zero: the leftmost index itself. `0` when nothing is stored above zero |
| `get_minus_dimension(&self) -> i64` | How far the vector reaches below zero: the rightmost index, negated. `0` when nothing is stored below zero |
| `get(&self, index: i64) -> T` | The value at `index`, or the default |
| `set(&mut self, index: i64, value: T) -> &mut Self` | Inserts or updates the entry, and returns the vector itself, so calls chain |
| `reset_value(&mut self, index: i64) -> Option<T>` | Resets the entry back to the default, returning it if there was one |
| `reset_vector(&mut self)` | Resets every entry, keeping the default |
| `elements(&self) -> Vec<Element<T>>` | Every entry, in descending index order |
| `inverted_elements(&self) -> Vec<Element<T>>` | The same entries in ascending index order |
| `indexes(&self) -> Vec<i64>` | Every stored index, descending |
| `inverted_indexes(&self) -> Vec<i64>` | The same indices in ascending order |
| `values(&self) -> Vec<T>` | Every stored value, in descending index order |
| `inverted_values(&self) -> Vec<T>` | The same values in ascending index order |
| `element(&self, n: usize) -> Element<T>` | The (n+1)-th entry from the left, i.e. `elements()[n]` |
| `element_index(&self, n: usize) -> i64` | The index that entry sits at |
| `element_value(&self, n: usize) -> T` | Its value |
| `inverted_element(&self, n: usize) -> Element<T>` | The same counting from the right, i.e. `inverted_elements()[n]`; `inverted_element(0)` is the rightmost entry |
| `inverted_element_index(&self, n: usize) -> i64` | The index that entry sits at |
| `inverted_element_value(&self, n: usize) -> T` | Its value |
| `left_significant_value(&self, n: i64) -> T` | The value `n` positions right of the first stored entry |
| `right_significant_value(&self, n: i64) -> T` | The value `n` positions left of the last stored entry |
| `iter(&self) -> Iter<'_, T>` | Borrows the stored entries in descending index order |
| `clone()` | An independent copy, default value included |
| `from_elements(elements, default) -> SparseVector<T>` | Builds from an `IntoIterator<Item = Element<T>>` |

Getting a value and listing require `T: Clone`; writing, resetting and changing the default require `T: PartialEq`

`index` is an `i64`, so a non-integer index does not compile

Reading a position outside the data range does not throw, it returns the default value: that is the point of this type

The `n` the ordinal methods take is a `usize`, so an ordinal cannot be negative and a negative literal does not compile; an ordinal past the end panics

The significant-value methods take a signed `i64`, so a negative `n` walks the other way, into the default value. They panic when the vector holds no entry at all, and also when the position would leave the `i64` range

`&SparseVector<T>` is also `IntoIterator`, so `for element in &vector` works and yields `Element<&T>`

`SparseVector<T>` implements `Debug` when `T` does, and deliberately has **no `PartialEq`**: this project has no equality semantics, and inventing some would be a decision the other implementations never made

## Differences from the TypeScript implementation

The semantics are shared, but not everything carries over. Stated here rather than papered over.

**Indices are `i64`, so a non-integer index does not typecheck.** The TypeScript version takes a `number` and throws a `TypeError` for `1.5`, `NaN` or `Infinity`. Here those are compile errors, which is the better failure. The index domain is also strictly larger: TypeScript stops at `Number.MAX_SAFE_INTEGER` (2^53−1), this goes to the full `i64` range.

**Pruning compares with `PartialEq`.** For `f64` that is identical to the TypeScript behaviour, `NaN` caveat included: `NaN != NaN`, so a stored `NaN` survives even when the default is also `NaN`.

For your own types it can differ. TypeScript's `===` compares objects by *reference*; Rust's `PartialEq` is almost always *structural*. Two distinct objects with equal contents are one value there and two values here, so a position holding an equal-but-distinct object is pruned here and kept there. If you need reference identity, make it part of the type's equality — a newtype is the whole recipe:

```rust
use std::rc::Rc;

#[derive(Clone)]
struct ByPointer<T>(Rc<T>);

impl<T> PartialEq for ByPointer<T> {
    fn eq(&self, other: &Self) -> bool {
        Rc::ptr_eq(&self.0, &other.0)
    }
}
```

**Nothing is omitted by accident.** `SparseVector::default()` uses `T::default()`, not a literal `0`. For `f64` that is `0.0`, exactly the documented rule; for `String` it is `""`, which is the same rule generalised rather than a lie about the type.

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
