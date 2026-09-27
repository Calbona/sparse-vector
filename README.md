**English** · [Deutsch](docs/de-DE.md) · [Español](docs/es-ES.md) · [Français](docs/fr-FR.md) · [Italiano](docs/it-IT.md) · [日本語](docs/ja-JP.md) · [한국어](docs/ko-KR.md) · [Русский](docs/ru-RU.md) · [Tiếng Việt](docs/vi-VN.md) · [简体中文](docs/zh-CN.md) · [繁體中文](docs/zh-TW.md)

# sparse-vector

## Overview

- **A sparse vector is not a vector**

	Its "index" does not start at zero and does not run left to right. It works the way a number is written by hand, the index standing in for the positional weight, so it reaches from positive infinity down to negative infinity. No language has infinities, of course: it is a `number` in TypeScript, an `int64_t` in C++ and an `i64` in Rust.

	And since it is not a mathematical vector, it carries no arithmetic. In that sense it is also a dictionary, and every position really does hold data of any type.

- **How a sparse vector is built**

	Only the positions that differ from the default value are stored.

	So take several objects called "entries", each an index paired with a value that differs from the default, lay them out as a list, add the default value, and there it is: a sparse vector. Wherever the indices fall, k entries means O(k) memory.

## Lib

| Language | Package | Version | Status | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | released | [`typescript/`](typescript/) |
| C++ | `sparse-vector` | 2.0.0 | released | [`c++/`](c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | released | [`rust/`](rust/) |

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

### The default value of an empty position

- A sparse vector is created with a default value

- That default value can be replaced later

- Every position holds a defined value, so reading always has an answer: any integer returns a value

### An entry equal to the default value is never kept

- Writing the default value into a position is the same as erasing whatever was there

- Replacing the default value immediately drops the entries equal to it

- That is the principle that keeps the structure sparse

### Equality

- Whether an entry equals the default value follows each language's ordinary practice: `===` in TypeScript, `operator==` in C++, `PartialEq` in Rust

- The awkward cases:
	- `-0.0` equals `0.0`
	- `NaN` does not equal itself
	- `0`, `'0'`, `false` and `null` are four values of four types
	- `===` compares object references, while `operator==` and `PartialEq` compare structure: two distinct objects with equal contents are one value in TypeScript and two in C++ and Rust, so the first keeps them and the other two drop them

- When you need identity, build it into the type's own equality, and a pointer type gives it to you directly: `std::shared_ptr`'s `operator==` compares pointers, and an `Rc<T>` can be wrapped in a newtype comparing with `Rc::ptr_eq` — the C++ and Rust READMEs each give that recipe

## License

MIT
