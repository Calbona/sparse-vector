**English** · [简体中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-CN.md) · [繁體中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-TW.md) · [日本語](https://github.com/Calbona/sparse-vector/blob/main/docs/ja-JP.md) · [Español](https://github.com/Calbona/sparse-vector/blob/main/docs/es-ES.md) · [Français](https://github.com/Calbona/sparse-vector/blob/main/docs/fr-FR.md) · [Русский](https://github.com/Calbona/sparse-vector/blob/main/docs/ru-RU.md)

# sparse-vector-rs

Rust implementation of [sparse-vector](https://github.com/Calbona/sparse-vector#readme): a mapping from integer index to arbitrary value that stores only the positions differing from a default value

[Installation](https://github.com/Calbona/sparse-vector#installation) and the shared semantics live in the [project README](https://github.com/Calbona/sparse-vector#readme); this page covers the Rust API

## Usage

```rust
use sparse_vector::SparseVector;

let mut vector = SparseVector::with_default(""); // empty positions read back as ""

vector.set(1_000_000, "far away");
vector.set(-42, "negative");

assert_eq!(vector.get(1_000_000), "far away");
assert_eq!(vector.get(-42), "negative");
assert_eq!(vector.get(7), ""); // no entry there, so the default value
assert_eq!(vector.len(), 2);
```

`SparseVector::new()` is the same thing for the default `f64`, starting from `0.0`:

```rust
use sparse_vector::SparseVector;

let vector = SparseVector::new();
assert_eq!(vector.default_value(), &0.0);
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

### `SparseVector<T = f64>`

| Member | Description |
| --- | --- |
| `new() -> SparseVector<f64>` | A vector whose default is `0.0`. Defined on `SparseVector<f64>` only — for another `T`, use `with_default` |
| `with_default(default: T) -> SparseVector<T>` | A vector whose empty positions read back as `default` |
| `default_value(&self) -> &T` | The value reported for positions with no explicit entry |
| `set_default_value(&mut self, next: T)` | Replaces the default, immediately pruning every entry equal to it. Requires `T: PartialEq` |
| `len(&self) -> usize` | Number of explicitly stored entries |
| `is_empty(&self) -> bool` | Whether nothing is stored |
| `contains_key(&self, index: i64) -> bool` | Whether `index` holds an explicit entry |
| `get(&self, index: i64) -> T` | The value at `index`, or the default. Requires `T: Clone` |
| `set(&mut self, index: i64, value: T) -> &mut Self` | Inserts or updates; chainable. Requires `T: PartialEq` |
| `remove(&mut self, index: i64) -> Option<T>` | Removes the entry and returns it, if there was one. Requires `T: PartialEq` |
| `clear(&mut self)` | Removes every entry, keeping the default |
| `elements(&self) -> Vec<Element<T>>` | The stored entries, in ascending index order. Requires `T: Clone` |
| `keys(&self) -> Vec<i64>` | The stored indices, in ascending order |
| `values(&self) -> Vec<T>` | The stored values, in ascending index order. Requires `T: Clone` |
| `element(&self, n: usize) -> Element<T>` | The (n+1)-th stored entry from the left, i.e. `elements()[n]`. Requires `T: Clone` |
| `element_index(&self, n: usize) -> i64` | The index that entry sits at |
| `element_value(&self, n: usize) -> T` | Its value. Requires `T: Clone` |
| `last_element(&self, n: usize) -> Element<T>` | The same counting from the right, so `last_element(0)` is the rightmost entry. Requires `T: Clone` |
| `last_element_index(&self, n: usize) -> i64` | The index that entry sits at |
| `last_element_value(&self, n: usize) -> T` | Its value. Requires `T: Clone` |
| `left_significant_value(&self, n: i64) -> T` | The value `n` positions right of the first stored entry. Requires `T: Clone` |
| `right_significant_value(&self, n: i64) -> T` | The value `n` positions left of the last stored entry. Requires `T: Clone` |
| `iter(&self) -> Iter<'_, T>` | Borrows the stored entries in ascending index order |
| `from_elements(elements, default) -> SparseVector<T>` | Builds from an `IntoIterator<Item = Element<T>>`. Requires `T: PartialEq` |

`&SparseVector<T>` is also `IntoIterator`, so `for element in &vector` works and yields `Element<&T>`. `SparseVector<T>` implements `Default` whenever `T` does, which is the `T`-generic spelling of `with_default(T::default())`.

`SparseVector<T>` implements `Clone` (an independent copy, default included) and `Debug` when `T` does. It deliberately has **no `PartialEq`**: this project has no equality semantics, and inventing some would be a decision the other implementations never made.

The `n` the ordinal methods take numbers the *entries*, not the positions: `element(0)` is the leftmost stored entry, however far out its index lies. It is a `usize`, so an ordinal cannot be negative and a negative literal does not compile; an ordinal past the end panics. These walk the map rather than going through `elements()`, so reading one entry does not clone the whole array:

```rust
use sparse_vector::{Element, SparseVector};

let mut vector = SparseVector::with_default("");
vector.set(10, "a");
vector.set(13, "d");

assert_eq!(vector.element(0), Element { index: 10, value: "a" }); // the first entry stored
assert_eq!(vector.element_value(1), "d");
assert_eq!(vector.last_element_value(0), "d"); // the last entry stored
```

The significant pair asks the other kind of question: it measures *positions*, and takes a signed `i64`. The first stored entry stands in for the first significant digit, the last one for the last, and the empty positions in between count the way the zeros inside a number count:

```rust
use sparse_vector::SparseVector;

let mut vector = SparseVector::with_default("");
vector.set(10, "a");
vector.set(13, "d");

assert_eq!(vector.left_significant_value(0), "a"); // index 10
assert_eq!(vector.left_significant_value(2), "");  // index 12, an empty position
assert_eq!(vector.left_significant_value(3), "d"); // index 13
```

A negative `n` walks the other way, into the default value. These panic when the vector holds no entry at all, there being no position to measure from, and also when the position would leave the `i64` range, which is checked rather than wrapped.

### `Element<T>`

```rust
use sparse_vector::Element;

let element = Element { index: -3, value: "a" };
assert_eq!(element.index, -3);
assert_eq!(element.value, "a");
```

The serialization shape described in the [project README](https://github.com/Calbona/sparse-vector#serialization). A round trip carries the default alongside, since it is not part of the shape:

```rust
use sparse_vector::SparseVector;

let mut original = SparseVector::with_default("none");
original.set(-1, "a");
original.set(10, "b");

let restored = SparseVector::from_elements(original.elements(), "none");

assert_eq!(restored.elements(), original.elements());
assert_eq!(restored.get(0), "none");
```

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

**Nothing is omitted by accident.** `SparseVector::new()` uses `T::default()`, not a literal `0`. For `f64` that is `0.0`, exactly the documented rule; for `String` it is `""`, which is the same rule generalised rather than a lie about the type.

**There is no JSON encoder here**, for the same reason there is none in the C++ implementation: `elements()` is the contract, and serialization is left to whatever crate you already use rather than taken on as a dependency.

## Development

```sh
cargo test          # unit tests, integration tests and the README examples
cargo build         # build the library
cargo doc --no-deps --open
```

The library itself has no dependencies — not even for the tests

## License

MIT
