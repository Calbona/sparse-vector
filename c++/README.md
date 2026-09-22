**English** · [简体中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-CN.md) · [繁體中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-TW.md) · [日本語](https://github.com/Calbona/sparse-vector/blob/main/docs/ja-JP.md) · [Español](https://github.com/Calbona/sparse-vector/blob/main/docs/es-ES.md) · [Français](https://github.com/Calbona/sparse-vector/blob/main/docs/fr-FR.md) · [Русский](https://github.com/Calbona/sparse-vector/blob/main/docs/ru-RU.md)

# sparse-vector for C++

Header-only C++17 implementation of [sparse-vector](https://github.com/Calbona/sparse-vector#readme): a mapping from integer index to arbitrary value that stores only the positions differing from a default value

Not yet in vcpkg or Conan. [Installation](https://github.com/Calbona/sparse-vector#installation) and the shared semantics live in the [project README](https://github.com/Calbona/sparse-vector#readme); this page covers the C++ API

## Usage

```cpp
#include <sv/SV_vector.hpp>

sv::SV_vector<std::string> vector(std::string(""));  // empty positions read back as ""

vector.set(1'000'000, "far away");
vector.set(-42, "negative");

vector.get(1'000'000);  // "far away"
vector.get(-42);        // "negative"
vector.get(7);          // "" — no entry there, so the default value
vector.size();          // 2
```

`sv::SV_vector<>` is the `double` case, where the default value starts at `0.0`:

```cpp
sv::SV_vector<> vector;
vector.default_value();     // 0.0
vector.get(1'000'000);      // 0.0
```

The default value is replaceable, and replacing it prunes immediately:

```cpp
sv::SV_vector<int> vector(0);
vector.set(1, 7);
vector.set(2, 9);

vector.set_default_value(7);
// Position 1 held 7, so it is dropped; position 2 keeps its value.
vector.size();              // 1
```

The type of the values is not restricted — anything you can compare and copy works:

```cpp
#include <optional>

sv::SV_vector<std::optional<int>> vector(std::nullopt);
vector.set(3, 1);

vector.get(3);              // std::optional<int>{1}
vector.get(4);              // std::nullopt — empty positions are empty
```

## API

```cpp
namespace sv {

using index_type = std::int64_t;

template <class T = double>
struct SV_element { index_type index; T value; };

template <class T = double>
class SV_vector;

}
```

### `sv::SV_vector<T = double>`

| Member | Description |
| --- | --- |
| `SV_vector()` | A vector whose default is `T{}`. For `double` that is `0.0` |
| `explicit SV_vector(const T& default)` | A vector whose empty positions read back as `default` |
| `default_value() const -> const T&` | The value reported for positions with no explicit entry. No non-const overload — see below |
| `set_default_value(const T& next)` | Replaces the default, immediately pruning every entry equal to it |
| `size() const -> std::size_t` | Number of explicitly stored entries |
| `empty() const -> bool` | Whether nothing is stored |
| `has(index_type) const -> bool` | Whether `index` holds an explicit entry |
| `get(index_type) const -> T` | The value at `index`, or the default. Returned by value |
| `set(index_type, const T&) -> SV_vector&` | Inserts or updates; chainable |
| `erase(index_type) -> bool` | Removes the entry, returning whether there was one |
| `clear()` | Removes every entry, keeping the default |
| `elements() const -> std::vector<SV_element<T>>` | The stored entries, in ascending index order |
| `keys() const -> std::vector<index_type>` | The stored indices, in ascending order |
| `values() const -> std::vector<T>` | The stored values, in ascending index order |
| `element(std::size_t) const -> SV_element<T>` | The (n+1)-th stored entry from the left, i.e. `elements()[n]` |
| `element_index(std::size_t) const -> index_type` | The index that entry sits at |
| `element_value(std::size_t) const -> T` | Its value |
| `last_element(std::size_t) const -> SV_element<T>` | The same counting from the right, so `last_element(0)` is the rightmost entry |
| `last_element_index(std::size_t) const -> index_type` | The index that entry sits at |
| `last_element_value(std::size_t) const -> T` | Its value |
| `left_significant_value(index_type) const -> T` | The value `n` positions right of the first stored entry |
| `right_significant_value(index_type) const -> T` | The value `n` positions left of the last stored entry |
| `begin()` / `end() const` | Borrows the stored entries in ascending index order |
| `from_elements(first, last[, default])` | Builds from any input iterator over `SV_element<T>` |
| `from(const std::vector<SV_element<T>>&[, default])` | The same, for a vector of elements |

Reading a stored entry without copying it is what `begin()`/`end()` are for:

```cpp
for (const auto& [index, value] : vector) {
  // ascending index order
}
```

The `n` the ordinal methods take numbers the *entries*, not the positions: `element(0)` is the leftmost stored entry, however far out its index lies. It is a `std::size_t`, so an ordinal cannot be negative and a negative literal does not compile; an ordinal past the end throws `std::out_of_range`. These walk the map rather than going through `elements()`, so reading one entry does not copy the whole array

```cpp
sv::SV_vector<std::string> vector(std::string(""));
vector.set(10, "a");
vector.set(13, "d");

vector.element(0);             // {index: 10, value: "a"} — the first entry stored
vector.element_value(1);       // "d"
vector.last_element_value(0);  // "d" — the last entry stored
```

The significant pair asks the other kind of question: it measures *positions*, and takes a signed `index_type`. The first stored entry stands in for the first significant digit, the last one for the last, and the empty positions in between count the way the zeros inside a number count

```cpp
vector.left_significant_value(0);   // "a"  — index 10
vector.left_significant_value(2);   // ""   — index 12, an empty position
vector.left_significant_value(3);   // "d"  — index 13
```

A negative `n` walks the other way, into the default value. These throw `std::out_of_range` when the vector holds no entry at all, there being no position to measure from, and also when the position would leave the `index_type` range, which signed overflow would otherwise make undefined

The copy constructor **is** `clone()` — there is no member function of that name, because a C++ value copy is already an independent one, default value included:

```cpp
sv::SV_vector<std::string> copy = vector;
```

`SV_vector` deliberately has **no `operator==`**: this project has no equality semantics, and inventing some would be a decision the other implementations never made.

`default_value()` returns `const T&` and has no non-const overload. A mutable reference would let a caller change the default without pruning, silently breaking the invariant that no stored entry equals it. Use `set_default_value()`.

### `sv::SV_element<T>`

An aggregate, so brace initialization works:

```cpp
sv::SV_element<double> element{1, 2.0};
element.index;   // 1
element.value;   // 2.0
```

The serialization shape described in the [project README](https://github.com/Calbona/sparse-vector#serialization). A round trip carries the default alongside, since it is not part of the shape:

```cpp
sv::SV_vector<std::string> original(std::string("none"));
original.set(-1, "a");
original.set(10, "b");

const sv::SV_vector<std::string> restored =
    sv::SV_vector<std::string>::from(original.elements(), "none");

restored.elements() == original.elements();  // true
restored.get(0);                             // "none"
```

## Differences from the TypeScript implementation

The semantics are shared, but not everything carries over. Stated here rather than papered over.

**Indices are `std::int64_t`, so a non-integer index does not compile.** The TypeScript version takes a `number` and throws a `TypeError` for `1.5`, `NaN` or `Infinity`. Here those are compile errors, which is the better failure. The index domain is also strictly larger: TypeScript stops at `Number.MAX_SAFE_INTEGER` (2^53−1), this goes to the full `int64_t` range.

**Pruning compares with `operator==`.** For `double` that is identical to the TypeScript behaviour, `NaN` caveat included: `NaN != NaN`, so a stored `NaN` survives even when the default is also `NaN`.

For your own types it can differ. TypeScript's `===` compares objects by *reference*; a C++ `operator==` is almost always *structural*. Two distinct objects with equal contents are one value there and two values here, so a position holding an equal-but-distinct object is pruned here and kept there. If you want reference identity, `std::shared_ptr` gives it to you — its `operator==` compares the pointers, not the pointees:

```cpp
std::shared_ptr<const Widget> first = std::make_shared<const Widget>(...);
std::shared_ptr<const Widget> second = std::make_shared<const Widget>(...);  // equal contents

sv::SV_vector<std::shared_ptr<const Widget>> vector(first);
vector.set(1, second);   // a different pointer, so it is kept
vector.set(2, first);    // the same pointer as the default, so it is dropped
```

**Nothing is omitted by accident.** The default constructor uses `T{}`, not a literal `0`. For `double` that is `0.0`, exactly the documented rule; for `std::string` it is `""`, which is the same rule generalised rather than a lie about the type.

**There is no JSON encoder here**, for the same reason there is none in the Rust implementation: `elements()` is the contract, and a hand-rolled writer would add an escaping surface for no gain. Serialize the elements with whatever library you already use.

## Development

```sh
cmake -S . -B build -G Ninja      # or any generator you like
cmake --build build
ctest --test-dir build --output-on-failure
```

The header-only library has no dependencies, and neither do its tests: `test/sv_test.hpp` is a small assertion harness written for this project rather than a framework, and it is never installed, since only `include/` is. Consuming the project as a subproject is detected, and the tests are then not built.

See [`test/`](test/) for the suite. Its cases mirror the TypeScript suite one for one by name, so the implementations can be diffed against each other.

## License

MIT
