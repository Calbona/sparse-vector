**English** · [Deutsch](docs/de-DE.md) · [Español](docs/es-ES.md) · [Français](docs/fr-FR.md) · [Italiano](docs/it-IT.md) · [日本語](docs/ja-JP.md) · [한국어](docs/ko-KR.md) · [Русский](docs/ru-RU.md) · [Tiếng Việt](docs/vi-VN.md) · [简体中文](docs/zh-CN.md) · [繁體中文](docs/zh-TW.md)

# sparse-vector

## Overview

- **What a sparse vector is**

	A sparse vector is not a mathematical vector. It reads the way a number is written by hand, its "index" reaching from positive infinity down to negative infinity. No language has infinities, of course: it is a `number` in TypeScript, an `int64_t` in C++ and an `i64` in Rust.

	Not being a vector, it carries no arithmetic.

- **How one works**

	Ask for the value at any positional weight and you get an answer — how? Only the positions differing from the default value are stored, plus the default itself. Ask about a position nothing was stored at and the vector hands you the default.

## Lib

| Language | Package | Version | Status | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | released | [`typescript/`](typescript/) |
| C++ | `sparse-vector` | 3.0.0 | released | [`c++/`](c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | released | [`rust/`](rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*not yet in vcpkg or Conan*

point CMake at the repository:

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

a checkout sitting beside your project works the same way, with `add_subdirectory(path/to/sparse-vector/c++)`
an install prefix exports a CMake package too, so `find_package(sparse-vector)` works

### Rust

```sh
cargo add sparse-vector-rs
```

## Semantics

### Vector

- Not a mathematical vector, and not a computer array, but the data structure this library provides.

### Sparse

- The capacity far exceeds the element count: some positional weights were never explicitly stored.

### Index

- Every integer, positive or negative. It stands for something like the units or the tens place.

### Value

- What we actually want to store, the counterpart of the digit in the hundreds or the thousands place — except that the type need not be a number. It can be anything.

### Element

- One index plus one value. That object is an element, and elements are what the vector really stores.

### Default value

- Every position with nothing explicitly stored holds the default value. Think of writing a number: you leave the zeros out, you write 1 rather than 0001.000.

- Once the vector is built, the default value can be replaced. Strange, who knows what for, but the ability is there.

### Minimal memory, kept automatically

- Replacing the default value immediately drops the elements equal to it.

- Replacing the equality predicate does the same.

- Writing the default value into a position erases whatever was there.

### Equality

- Is a value equal to the default? By each language's ordinary test.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- You can also hand the vector a predicate — the value under test and the current default, returning a boolean — and it replaces the ordinary test.

- Comparing two vectors is two named operations, and not the same thing as pruning: `isEqualTo` / `is_equal_to` decides whether two vectors are equal, `differences` lists the elements where the receiver differs from the other. Both **answer under the receiver's predicate**, so two vectors carrying different predicates can make `a.isEqualTo(b)` and `b.isEqualTo(a)` disagree.

- These ordinary tests have some counter-intuitive corners.
	- `NaN` does not equal itself.
	- `0`, `'0'`, `false` and `null` are of four types, so they are not equal.
	- `===` compares object references, while `operator==` and `PartialEq` compare structure.

## License

MIT
