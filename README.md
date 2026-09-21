**English** · [简体中文](docs/zh-CN.md) · [繁體中文](docs/zh-TW.md) · [日本語](docs/ja-JP.md) · [Español](docs/es-ES.md) · [Français](docs/fr-FR.md) · [Русский](docs/ru-RU.md)

# sparse-vector

A sparse vector for TypeScript: a mapping from integer indices to arbitrary values, storing only the positions that differ from a default value

Indices may be negative, and they need not be contiguous

So a vector with only three entries occupies only three entries — whether those three indices are `0, 1, 2` or `-10^9, 0, 10^9`

This library is deliberately **not** a vector in the mathematical sense

## Installation

```sh
npm install sparse-vector
```

## Usage

```ts
import { SV_vector } from 'sparse-vector';

const vector = new SV_vector(); // default value for empty positions: the number 0

vector.set(1_000_000, 'far away');
vector.set(-42, 'negative');

vector.get(1_000_000); // 'far away'
vector.get(-42);       // 'negative'
vector.get(7);         // 0
vector.size;           // 2
```

The default value can be given, and changed later; every position without an explicit entry reads back as it

```ts
const counts = new SV_vector<number | null>(null); // empty positions are null
counts.set(3, 1);
counts.get(4); // null

counts.defaultValue = 0; // empty positions are now 0
```

Values are unrestricted; anything goes

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

## API

### `new SV_vector<T>(defaultValue?)`

Creates a vector

`defaultValue` is the default value for empty positions, i.e. the value reported for every position without an explicit entry; it defaults to the number `0`

`T` defaults to `number`

### Properties

| Member | Description |
| --- | --- |
| `defaultValue: T` | Readable and writable; assigning it immediately drops every entry strictly equal to the new default |
| `size: number` | Number of explicitly stored entries |

### Methods

| Method | Description |
| --- | --- |
| `get(index): T` | The value at `index`; returns `defaultValue` when no explicit entry is there |
| `set(index, value): this` | Insert or update, chainable |
| `has(index): boolean` | Whether an explicit entry exists at `index` |
| `delete(index): boolean` | Remove the explicit entry there, back to the default value |
| `clear(): void` | Remove every explicit entry, back to the default value |
| `elements(): SV_element<T>[]` | The explicit entries, in ascending index order |
| `keys(): number[]` | The stored indices, ascending |
| `values(): T[]` | The stored values, in ascending index order |
| `clone(): SV_vector<T>` | An independent copy |
| `[Symbol.iterator]()` | Iterates the explicit entries in ascending index order |
| `toJSON(): SV_element<T>[]` | Same as `elements()`, so `JSON.stringify` works directly |

`index` must be an integer — positive or negative; a non-integer throws `TypeError`

Reading a position outside the data range does not throw, it returns the default value: that is the whole point of this type

### `SV_vector.from(elements, defaultValue?)`

Builds from an iterable of `SV_element`; entries strictly equal to the default are dropped; for a repeated index the last one wins

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

This is the serialization format: a plain JSON object with exactly the two keys `index` and `value`

Note that `toJSON()` outputs the entries only, the default value is not part of this structure, so carry it along on a JSON round trip

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

## Pruning rules

An entry equal to the default value for empty positions is never kept

Pruning uses **strict equality**, which has two consequences:

- `0`, `'0'`, `false` and `null` are four different values; only a `===` match is dropped
- `NaN` is not strictly equal to `NaN`, so a stored `NaN` is kept even when the default value is itself `NaN`

## Development

```sh
npm run build      # compile to dist/
npm run typecheck  # typecheck src and tests
npm test           # build first, then test
```

Requires Node 24+

The library itself has no runtime dependencies

## License

MIT
