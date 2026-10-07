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

The default value can be given, and changed later; every position without an explicit element reads back as it

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

A predicate replaces `===` wherever the default value is compared, pruning included

```ts
const vector = new SV_vector<string>('ab');
vector.setEquality = (value, defaultValue) => value.length === defaultValue.length;

vector.set(1, 'xy'); // a different string of the same length
vector.getElementAmount; // 0
```

Two vectors are compared as a whole by `isEqualTo`, and what differs is listed by `differences`. Both decide every comparison with the **receiver's** predicate

```ts
const a = new SV_vector<number>(0);
a.set(1, 7);
const b = new SV_vector<number>(0);
b.set(1, 7);
b.set(2, 9);

a.isEqualTo(b);   // false — position 2 differs
a.differences(b); // [{ index: 2, value: 0 }] — the receiver's value there, i.e. its default
```

## API

| Member | Description |
| --- | --- |
| `new SV_vector()` | Builds a sparse vector, the default left out, which is then the number `0` |
| `new SV_vector(defaultValue)` | Builds a sparse vector |
| `SV_equality<T>` | The predicate type: `(value: T, defaultValue: T) => boolean`, called with the value under test first and the current default second |
| `getDefaultValue` | The value reported for every position without an explicit element |
| `setDefaultValue = next` | Assignable; assigning it immediately drops every element strictly equal to the new default |
| `getEquality` | The predicate replacing `===`, or `undefined` when `===` decides |
| `setEquality = next` | Assignable; assigning it immediately drops every element it calls equal to the default. `undefined` restores `===` |
| `getElementAmount` | Number of explicitly stored elements |
| `getSignificantDimension` | The distance between the leftmost and rightmost elements, both included — `-2` and `5` are `8`. `0` when nothing is stored |
| `getPlusDimension` | How far the vector reaches above zero: the leftmost index itself. `0` when nothing is stored above zero |
| `getMinusDimension` | How far the vector reaches below zero: the rightmost index, negated. `0` when nothing is stored below zero |
| `get(index)` | The value at `index`; the default when no explicit element is there |
| `set(index, value)` | Inserts or updates the element, and returns `this`, so calls chain |
| `resetValue(index)` | Resets the element back to the default, returning whether there was one |
| `resetVector()` | Resets every element, keeping the default, returning whether there was any |
| `elements()` | Every element, in descending index order |
| `invertedElements()` | The same elements in ascending index order |
| `indexes()` | Every stored index, descending |
| `invertedIndexes()` | The same indices in ascending order |
| `values()` | Every stored value, in descending index order |
| `invertedValues()` | The same values in ascending index order |
| `element(n)` | The (n+1)-th element from the left, i.e. `elements()[n]` |
| `elementIndex(n)` | The index that element sits at |
| `elementValue(n)` | Its value |
| `invertedElement(n)` | The same counting from the right, i.e. `invertedElements()[n]`; `invertedElement(0)` is the rightmost element |
| `invertedElementIndex(n)` | The index that element sits at |
| `invertedElementValue(n)` | Its value |
| `leftSignificantValue(n)` | The value `n` positions right of the first stored element |
| `rightSignificantValue(n)` | The value `n` positions left of the last stored element |
| `[Symbol.iterator]()` | Iterates every element in descending index order |
| `clone()` | An independent copy, equality predicate included |
| `SV_vector.fromElements(elements, defaultValue?)` | Builds from an iterable of `SV_element`; elements strictly equal to the default are dropped, and for a repeated index the last one wins |
| `isEqualTo(other)` | Whether `other` holds the same values as this vector, every comparison made by this vector's predicate. A position holding nothing contributes its own vector's default. Never throws |
| `differences(other)` | The elements of this vector that differ from `other`, descending by index, each carrying this vector's value there — the default value itself where nothing is stored. Throws `TypeError` unless the two default values are of the same type and compare equal |

`index` must be an integer — positive or negative; a non-integer throws `TypeError`

Reading a position outside the data range does not throw, it returns the default value: that is the point of this type

The `n` the ordinal methods take is 0-based; an ordinal past the end, or a negative one, throws `RangeError`, and a non-integer throws `TypeError`

The significant-value methods take a signed offset, so a negative `n` walks the other way, into the default value. They throw `RangeError` only when the vector holds no element at all

Both comparison methods trust the predicate rather than correcting it: an asymmetric one makes `a.isEqualTo(b)` and `b.isEqualTo(a)` disagree, a non-reflexive one makes a vector unequal to itself, and a `NaN` default makes both methods treat a vector as unequal to itself — `differences` refusing outright, since the precondition itself fails. The two also fail differently on an unequal default: `isEqualTo` returns `false`, `differences` throws

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
