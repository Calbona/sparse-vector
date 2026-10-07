**English** · [Deutsch](https://github.com/Calbona/sparse-vector/blob/main/docs/de-DE.md) · [Español](https://github.com/Calbona/sparse-vector/blob/main/docs/es-ES.md) · [Français](https://github.com/Calbona/sparse-vector/blob/main/docs/fr-FR.md) · [Italiano](https://github.com/Calbona/sparse-vector/blob/main/docs/it-IT.md) · [日本語](https://github.com/Calbona/sparse-vector/blob/main/docs/ja-JP.md) · [한국어](https://github.com/Calbona/sparse-vector/blob/main/docs/ko-KR.md) · [Русский](https://github.com/Calbona/sparse-vector/blob/main/docs/ru-RU.md) · [Tiếng Việt](https://github.com/Calbona/sparse-vector/blob/main/docs/vi-VN.md) · [简体中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-CN.md) · [繁體中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-TW.md)

# sparse-vector for C++

Header-only C++17 implementation of [sparse-vector](https://github.com/Calbona/sparse-vector#readme) — a mapping from integer index to arbitrary value that stores only the positions differing from a default value

## Installation

*Not yet in vcpkg or Conan*

Point CMake at the repository:

```cmake
include(FetchContent)

FetchContent_Declare(sparse-vector
  GIT_REPOSITORY https://github.com/Calbona/sparse-vector
  GIT_TAG main
  SOURCE_SUBDIR c++
)
FetchContent_MakeAvailable(sparse-vector)

target_link_libraries(your-target PRIVATE Calbona::sparse-vector)
```

A checkout sitting beside your project works the same way, with `add_subdirectory(path/to/sparse-vector/c++)`. An install prefix exports a CMake package too, so `find_package(sparse-vector)` works

## Quick start

```cpp
#include <sv/SV_vector.hpp>

sv::SV_vector<std::string> vector(std::string(""));  // empty positions read back as ""

vector.set(1'000'000, "far away");
vector.set(-42, "negative");

vector.get(1'000'000);  // "far away"
vector.get(-42);        // "negative"
vector.get(7);          // "" — no element there, so the default value
vector.get_element_amount();  // 2
```

`sv::SV_vector<>` is the `double` case, where the default value starts at `0.0`:

```cpp
sv::SV_vector<> vector;
vector.get_default_value(); // 0.0
vector.get(1'000'000);      // 0.0
```

The default value is replaceable, and replacing it prunes immediately:

```cpp
sv::SV_vector<int> vector(0);
vector.set(1, 7);
vector.set(2, 9);

// Position 1 held 7, so it is dropped; position 2 keeps its value.
vector.set_default_value(7);  // true, something was pruned
vector.get_element_amount();  // 1
```

The type of the values is not restricted — anything you can compare and copy works:

```cpp
#include <optional>

sv::SV_vector<std::optional<int>> vector(std::nullopt);
vector.set(3, 1);

vector.get(3);              // std::optional<int>{1}
vector.get(4);              // std::nullopt — empty positions are empty
```

A predicate replaces `operator==` wherever the default value is compared, pruning included:

```cpp
sv::SV_vector<std::string> vector(std::string("ab"));
vector.set_equality([](const std::string& value, const std::string& defaultValue) {
  return value.size() == defaultValue.size();
});

vector.set(1, "xy");  // a different string of the same length
vector.get_element_amount();  // 0
```

Two vectors are compared as a whole by `is_equal_to`, and what differs is listed by `differences`. Both decide every comparison with the **receiver's** predicate:

```cpp
sv::SV_vector<int> a(0);
a.set(1, 7);
sv::SV_vector<int> b(0);
b.set(1, 7);
b.set(2, 9);

a.is_equal_to(b);   // false — position 2 differs
a.differences(b);   // [{index: 2, value: 0}] — the receiver's value there, i.e. its default
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

| Member | Description |
| --- | --- |
| `SV_vector()` | Builds a sparse vector, the default left out, which is then `T{}` — `0.0` for `double` |
| `explicit SV_vector(const T& default)` | Builds a sparse vector |
| `equality_type` | The predicate type: `std::function<bool(const T&, const T&)>`, called with the value under test first and the current default second |
| `get_default_value() const -> const T&` | The value reported for positions with no explicit element |
| `set_default_value(const T& next) -> bool` | Replaces the default, immediately pruning every element equal to it, and returns whether anything was pruned |
| `get_equality() const -> const equality_type&` | The predicate replacing `operator==`, empty when there is none |
| `set_equality(equality_type next) -> bool` | Replaces the predicate, immediately pruning every element it calls equal to the default, and returns whether anything was pruned. An empty function restores `operator==` |
| `get_element_amount() const -> std::size_t` | Number of explicitly stored elements |
| `get_significant_dimension() const -> index_type` | The distance between the leftmost and rightmost elements, both included — `-2` and `5` are `8`. `0` when nothing is stored |
| `get_plus_dimension() const -> index_type` | How far the vector reaches above zero: the leftmost index itself. `0` when nothing is stored above zero |
| `get_minus_dimension() const -> index_type` | How far the vector reaches below zero: the rightmost index, negated. `0` when nothing is stored below zero |
| `get(index_type) const -> T` | The value at `index`, or the default. Returned by value |
| `set(index_type, const T&) -> SV_vector&` | Inserts or updates the element, and returns the vector itself, so calls chain |
| `reset_value(index_type) -> bool` | Resets the element back to the default, returning whether there was one |
| `reset_vector() -> bool` | Resets every element, keeping the default, returning whether there was any |
| `elements() const -> std::vector<SV_element<T>>` | Every element, in descending index order |
| `inverted_elements() const -> std::vector<SV_element<T>>` | The same elements in ascending index order |
| `indexes() const -> std::vector<index_type>` | Every stored index, descending |
| `inverted_indexes() const -> std::vector<index_type>` | The same indices in ascending order |
| `values() const -> std::vector<T>` | Every stored value, in descending index order |
| `inverted_values() const -> std::vector<T>` | The same values in ascending index order |
| `element(std::size_t) const -> SV_element<T>` | The (n+1)-th element from the left, i.e. `elements()[n]` |
| `element_index(std::size_t) const -> index_type` | The index that element sits at |
| `element_value(std::size_t) const -> T` | Its value |
| `inverted_element(std::size_t) const -> SV_element<T>` | The same counting from the right, i.e. `inverted_elements()[n]`; `inverted_element(0)` is the rightmost element |
| `inverted_element_index(std::size_t) const -> index_type` | The index that element sits at |
| `inverted_element_value(std::size_t) const -> T` | Its value |
| `left_significant_value(index_type) const -> T` | The value `n` positions right of the first stored element |
| `right_significant_value(index_type) const -> T` | The value `n` positions left of the last stored element |
| `begin()` / `end() const` | A forward iterator over the stored elements in descending index order, yielding `container_type::value_type` — a `std::pair<const index_type, T>` |
| `SV_vector(const SV_vector&)` | The copy constructor, which is this implementation's `clone()` — a C++ value copy is already independent, equality predicate included |
| `container_type` | The element container: `std::unordered_map<index_type, T>` |
| `from_elements(first, last[, default])` | Builds from any input iterator over `SV_element<T>` |
| `from_elements(const std::vector<SV_element<T>>&[, default])` | The same, for a vector of elements |
| `is_equal_to(const SV_vector&) const -> bool` | Whether `other` holds the same values as `*this`, every comparison made by this vector's predicate. A position holding nothing contributes its own vector's default. Never throws |
| `differences(const SV_vector&) const -> std::vector<SV_element<T>>` | The elements of `*this` that differ from `other`, descending by index, each carrying this vector's value there — the default value itself where nothing is stored. Throws `std::invalid_argument` unless the two default values compare equal |

`index` is an `index_type`, i.e. `std::int64_t`, so a non-integer index does not compile

Reading a position outside the data range does not throw, it returns the default value: that is the point of this type

The `n` the ordinal methods take is a `std::size_t`, so an ordinal cannot be negative and a negative literal does not compile; an ordinal past the end throws `std::out_of_range`

The significant-value methods take a signed `index_type`, so a negative `n` walks the other way, into the default value. They throw `std::out_of_range` when the vector holds no element at all, and also when the position would leave the `index_type` range

`get_default_value()` returns `const T&` and has no non-const overload. A mutable reference would let a caller change the default without pruning, silently breaking the invariant that no stored element equals it. Use `set_default_value()`

`get_equality()` likewise returns a reference, and the predicate is a `std::function`: an empty one means `operator==`, which is also how `set_equality(nullptr)` restores it

`SV_vector` has **no `operator==` overload**. Whole-vector comparison is the named `is_equal_to()` and `differences()`, and both answer under the **receiver's** predicate, so `a.is_equal_to(b)` and `b.is_equal_to(a)` may disagree when the two carry different predicates. The predicate governs pruning as well, but the two are separate questions: pruning is about one value against one vector's default, comparison is about two vectors

Both comparison methods trust the predicate rather than correcting it: an asymmetric one makes the two directions disagree, a non-reflexive one makes a vector unequal to itself, and a `NaN` default makes both methods treat a vector as unequal to itself — `differences` throwing outright, since the precondition itself fails. The two also fail differently on an unequal default: `is_equal_to` returns `false`, `differences` throws

`container_type` holds no order, so the descending order the API promises everywhere is served by a mutable index of pointers kept on top of it. Storing a new index or erasing one drops that index; overwriting a value in place leaves it alone. Three consequences: `begin()`, `end()` and `get_plus_dimension()` are not `noexcept`, because they may have to rebuild it; a `const SV_vector` is **not safe to read from two threads at once**, since the first ordered read writes; and a copy never inherits the index, whose pointers would aim at the original's storage

## Differences from the TypeScript implementation

The semantics are shared, but not everything carries over. Stated here rather than papered over.

**Indices are `std::int64_t`, so a non-integer index does not compile.** The TypeScript version takes a `number` and throws a `TypeError` for `1.5`, `NaN` or `Infinity`. Here those are compile errors, which is the better failure. The index domain is also strictly larger: TypeScript stops at `Number.MAX_SAFE_INTEGER` (2^53−1), this goes to the full `int64_t` range.

**Pruning compares with `operator==`, or with your own predicate.** For `double` that is identical to the TypeScript behaviour, `NaN` caveat included: `NaN != NaN`, so a stored `NaN` survives even when the default is also `NaN`.

For your own types it can differ. TypeScript's `===` compares objects by *reference*; a C++ `operator==` is almost always *structural*. Two distinct objects with equal contents are one value there and two values here, so a position holding an equal-but-distinct object is pruned here and kept there. Want reference identity? Hand the vector a predicate that says so:

```cpp
#include <memory>

std::shared_ptr<const Widget> first = std::make_shared<const Widget>(...);
std::shared_ptr<const Widget> second = std::make_shared<const Widget>(...);  // equal contents

sv::SV_vector<std::shared_ptr<const Widget>> vector(first);
vector.set_equality([](const std::shared_ptr<const Widget>& value,
                       const std::shared_ptr<const Widget>& defaultValue) {
  return value == defaultValue;  // shared_ptr's operator== already compares pointers
});

vector.set(1, second);  // a different pointer, so it is kept
vector.set(2, first);   // the same pointer as the default, so it is dropped
```

**Nothing is omitted by accident.** The default constructor uses `T{}`, not a literal `0`. For `double` that is `0.0`, exactly the documented rule; for `std::string` it is `""`, which is the same rule generalised rather than a lie about the type.

**There is no JSON encoder here**, nor in the other two implementations: `elements()` is the contract, and a hand-rolled writer would add an escaping surface for no gain. Serialize the elements with whatever library you already use.

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
