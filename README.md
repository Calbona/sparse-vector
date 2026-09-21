**English** · [简体中文](docs/zh-CN.md) · [繁體中文](docs/zh-TW.md) · [日本語](docs/ja-JP.md) · [Español](docs/es-ES.md) · [Français](docs/fr-FR.md) · [Русский](docs/ru-RU.md)

# sparse-vector

A sparse vector: a mapping from integer indices to arbitrary values, storing only the positions that differ from a default value

Indices may be negative, and they need not be contiguous

So a vector with only three entries occupies only three entries — whether those three indices are `0, 1, 2` or `-10^9, 0, 10^9`

This library is deliberately **not** a vector in the mathematical sense. It carries no arithmetic; it is a data structure, and only that

## Implementations

The same type is offered in three languages. They share every rule below — only the spelling differs

| Language | Package | Directory | Status |
| --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | [`typescript/`](typescript/) | released |
| C++ | — | [`c++/`](c++/) | planned |
| Rust | `sparse-vector-rs` | [`rust/`](rust/) | planned |

Each implementation directory holds its own README with that language's installation, usage and API reference. This page defines the semantics they have in common, so they do not have to be restated three times

## Semantics shared by every implementation

### The default value

A vector is created with a default value: the value reported for every position that holds no explicit entry. It defaults to the number `0` unless one is given

The default value can be replaced later. Replacing it immediately drops every entry that compares equal to the new default

Because every position has a defined value, reading is total. Any integer — stored or not, in range or far outside it — returns a value rather than raising

### An entry equal to the default is never kept

Writing the default value into a position is the same as removing whatever was there. This is what keeps the structure sparse: memory is O(k) in the number of entries that actually differ from the default, no matter how far apart the indices are or how negative they get

### Pruning uses strict equality

Each language prunes with its own strictest equality — `===` in TypeScript, `==` in C++, `PartialEq` in Rust. Two consequences:

- `0`, `'0'`, `false` and `null` are four different values; only an exact match drops an entry
- `NaN` never equals itself, so a stored `NaN` survives even when the default value is itself `NaN`

### Serialization

An entry is a plain object with exactly two keys:

| Key | Type | Meaning |
| --- | --- | --- |
| `index` | integer | the position, negative or not |
| `value` | anything | the value stored there |

A vector serializes to its entries alone, in ascending index order. The default value is not part of this shape, so carry it alongside on a round trip

## License

MIT
