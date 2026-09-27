**English** · [Deutsch](https://github.com/Calbona/sparse-vector/blob/main/docs/de-DE.md) · [Español](https://github.com/Calbona/sparse-vector/blob/main/docs/es-ES.md) · [Français](https://github.com/Calbona/sparse-vector/blob/main/docs/fr-FR.md) · [Italiano](https://github.com/Calbona/sparse-vector/blob/main/docs/it-IT.md) · [日本語](https://github.com/Calbona/sparse-vector/blob/main/docs/ja-JP.md) · [한국어](https://github.com/Calbona/sparse-vector/blob/main/docs/ko-KR.md) · [Русский](https://github.com/Calbona/sparse-vector/blob/main/docs/ru-RU.md) · [Tiếng Việt](https://github.com/Calbona/sparse-vector/blob/main/docs/vi-VN.md) · [简体中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-CN.md) · [繁體中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-TW.md)

# @calbona/sparse-vector

The TypeScript implementation of [sparse-vector](https://github.com/Calbona/sparse-vector#readme) — a mapping from integer indices to arbitrary values that stores only the positions differing from a default value

## Installation

```sh
npm install @calbona/sparse-vector
```

## Quick start

```ts
import { SV_vector } from '@calbona/sparse-vector';

const vector = new SV_vector(); // default value for empty positions: the number 0

vector.set(1_000_000, 'far away');
vector.set(-42, 'negative');

vector.get(1_000_000); // 'far away'
vector.get(-42);       // 'negative'
vector.get(7);         // 0
vector.getElementAmount; // 2
```

The default value can be given, and changed later; every position without an explicit entry reads back as it

```ts
const counts = new SV_vector<number | null>(null); // empty positions are null
counts.set(3, 1);
counts.get(4); // null

counts.setDefaultValue = 0; // empty positions are now 0
```

Values are unrestricted; anything goes

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

## API

| Member | Description |
| --- | --- |
| `new SV_vector()` | Builds a sparse vector, the default left out, which is then the number `0` |
| `new SV_vector(defaultValue)` | Builds a sparse vector |
| `getDefaultValue` | The value reported for every position without an explicit entry |
| `setDefaultValue = next` | Assignable; assigning it immediately drops every entry strictly equal to the new default |
| `getElementAmount` | Number of explicitly stored entries |
| `getSignificantDimension` | The distance between the leftmost and rightmost entries, both included — `-2` and `5` are `8`. `0` when nothing is stored |
| `getPlusDimension` | How far the vector reaches above zero: the leftmost index itself. `0` when nothing is stored above zero |
| `getMinusDimension` | How far the vector reaches below zero: the rightmost index, negated. `0` when nothing is stored below zero |
| `get(index)` | The value at `index`; the default when no explicit entry is there |
| `set(index, value)` | Inserts or updates the entry, and returns `this`, so calls chain |
| `resetValue(index)` | Resets the entry back to the default, returning whether there was one |
| `resetVector()` | Resets every entry, keeping the default |
| `elements()` | Every entry, in descending index order |
| `invertedElements()` | The same entries in ascending index order |
| `indexes()` | Every stored index, descending |
| `invertedIndexes()` | The same indices in ascending order |
| `values()` | Every stored value, in descending index order |
| `invertedValues()` | The same values in ascending index order |
| `element(n)` | The (n+1)-th entry from the left, i.e. `elements()[n]` |
| `elementIndex(n)` | The index that entry sits at |
| `elementValue(n)` | Its value |
| `invertedElement(n)` | The same counting from the right, i.e. `invertedElements()[n]`; `invertedElement(0)` is the rightmost entry |
| `invertedElementIndex(n)` | The index that entry sits at |
| `invertedElementValue(n)` | Its value |
| `leftSignificantValue(n)` | The value `n` positions right of the first stored entry |
| `rightSignificantValue(n)` | The value `n` positions left of the last stored entry |
| `[Symbol.iterator]()` | Iterates every entry in descending index order |
| `clone()` | An independent copy |
| `SV_vector.fromElements(elements, defaultValue?)` | Builds from an iterable of `SV_element`; entries strictly equal to the default are dropped, and for a repeated index the last one wins |

`index` must be an integer — positive or negative; a non-integer throws `TypeError`

Reading a position outside the data range does not throw, it returns the default value: that is the point of this type

The `n` the ordinal methods take is 0-based; an ordinal past the end, or a negative one, throws `RangeError`, and a non-integer throws `TypeError`

The significant-value methods take a signed offset, so a negative `n` walks the other way, into the default value. They throw `RangeError` only when the vector holds no entry at all

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
