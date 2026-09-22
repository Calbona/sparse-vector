**English** · [简体中文](docs/zh-CN.md) · [繁體中文](docs/zh-TW.md) · [日本語](docs/ja-JP.md) · [Español](docs/es-ES.md) · [Français](docs/fr-FR.md) · [Русский](docs/ru-RU.md)

# sparse-vector

A sparse vector: a mapping from integer indices to arbitrary values, storing only the positions that differ from a default value

Indices may be negative, and they need not be contiguous

So a vector with only three entries occupies only three entries — whether those three indices are `0, 1, 2` or `-10^9, 0, 10^9`

This library is deliberately **not** a vector in the mathematical sense. It carries no arithmetic; it is a data structure, and only that

## Implementations

The same type is offered in three languages. They share every rule below — only the spelling differs

| Language | Package | Version | Directory | Status |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 1.1.0 | [`typescript/`](typescript/) | released |
| C++ | `sparse-vector` | 1.1.0 | [`c++/`](c++/) | released |
| Rust | `sparse-vector-rs` | 1.1.0 | [`rust/`](rust/) | released |

Each package is versioned independently, so the numbers differ. All three implement the same semantics.

## Installation

**TypeScript**

```sh
npm install @calbona/sparse-vector
```

**C++** — not yet in vcpkg or Conan. Two headers and nothing to link, so point CMake at the repository:

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

**Rust** — note that the crate you import is `sparse_vector`, not the package name:

```sh
cargo add sparse-vector-rs
```

Each implementation directory holds its own README covering that language's usage and API reference. This page defines the semantics they share, so they do not have to be restated three times

## Semantics shared by every implementation

### The default value

A vector is created with a default value: the value reported for every position that holds no explicit entry. It defaults to the number `0` unless one is given

The default value can be replaced later. Replacing it immediately drops every entry that compares equal to the new default

Because every position has a defined value, reading is total. Any integer — stored or not, in range or far outside it — returns a value rather than raising

### An entry equal to the default is never kept

Writing the default value into a position is the same as removing whatever was there. This is what keeps the structure sparse: memory is O(k) in the number of entries that actually differ from the default, no matter how far apart the indices are or how negative they get

### Pruning uses each language's own equality

An entry is dropped when it compares equal to the default value, using the language's ordinary equality — `===` in TypeScript, `operator==` in C++, `PartialEq` in Rust

For numbers and strings the three agree exactly, the awkward cases included:

- `-0.0` equals `0.0`, so a stored `-0.0` is dropped when the default is `0.0`
- `NaN` never equals itself, so a stored `NaN` survives even when the default value is itself `NaN`

TypeScript's `0`, `'0'`, `false` and `null` are four values of four types, and only an exact match drops an entry. A statically typed vector holds a single `T`, so that particular set cannot arise in C++ or Rust — but the rule it illustrates, that equality is exact rather than coercing, holds in all three

For objects the three genuinely part ways, and this is the one place where a value's *identity* is visible. TypeScript's `===` compares object references; `operator==` and `PartialEq` are usually structural. Two distinct objects with equal contents are one value in TypeScript and two in C++ and Rust, so an entry holding an equal-but-distinct object is kept by the first and pruned by the other two. When identity is what you want, make it part of the type's equality — a pointer type gets there for free, since `std::shared_ptr`'s `operator==` compares pointers, and an `Rc<T>` can be wrapped in a newtype comparing with `Rc::ptr_eq`. The C++ and Rust READMEs each give that recipe

### Serialization

An entry is a plain object with exactly two keys:

| Key | Type | Meaning |
| --- | --- | --- |
| `index` | integer | the position, negative or not |
| `value` | anything | the value stored there |

A vector serializes to its entries alone, in ascending index order. The default value is not part of this shape, so carry it alongside on a round trip

## License

MIT
