**English** · [简体中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-CN.md) · [繁體中文](https://github.com/Calbona/sparse-vector/blob/main/docs/zh-TW.md) · [日本語](https://github.com/Calbona/sparse-vector/blob/main/docs/ja-JP.md) · [Español](https://github.com/Calbona/sparse-vector/blob/main/docs/es-ES.md) · [Français](https://github.com/Calbona/sparse-vector/blob/main/docs/fr-FR.md) · [Русский](https://github.com/Calbona/sparse-vector/blob/main/docs/ru-RU.md)

# @calbona/sparse-vector

The TypeScript implementation of [sparse-vector](https://github.com/Calbona/sparse-vector#readme) — a mapping from integer indices to arbitrary values that stores only the positions differing from a default value. That page carries the [installation steps](https://github.com/Calbona/sparse-vector#installation) and the semantics all three implementations share; this one covers the TypeScript API

## Usage

```ts
import { SV_vector } from '@calbona/sparse-vector';

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
| `element(n): SV_element<T>` | The (n+1)-th explicit entry from the left, i.e. `elements()[n]` |
| `elementIndex(n): number` | The index that entry sits at |
| `elementValue(n): T` | Its value |
| `lastElement(n): SV_element<T>` | The same counting from the right, so `lastElement(0)` is the rightmost entry |
| `lastElementIndex(n): number` | The index that entry sits at |
| `lastElementValue(n): T` | Its value |
| `leftSignificantValue(n): T` | The value `n` positions right of the first explicit entry |
| `rightSignificantValue(n): T` | The value `n` positions left of the last explicit entry |
| `clone(): SV_vector<T>` | An independent copy |
| `[Symbol.iterator]()` | Iterates the explicit entries in ascending index order |
| `toJSON(): SV_element<T>[]` | Same as `elements()`, so `JSON.stringify` works directly |

`index` must be an integer — positive or negative; a non-integer throws `TypeError`

Reading a position outside the data range does not throw, it returns the default value: that is the whole point of this type

The `n` the ordinal methods take is 0-based and numbers the *entries*, not the positions: `element(0)` is the leftmost stored entry, however far out its index lies. An ordinal past the end, or a negative one, throws `RangeError`; a non-integer throws `TypeError`. These reuse `elements()`, so they read a copy of one entry rather than of the whole array

```ts
const vector = new SV_vector<string>('');
vector.set(10, 'a');
vector.set(13, 'd');

vector.element(0);            // { index: 10, value: 'a' } — the first entry stored
vector.elementValue(1);       // 'd'
vector.lastElementValue(0);   // 'd' — the last entry stored
```

The significant pair is the other kind of question: it measures *positions*. The first stored entry stands in for the first significant digit, the last one for the last, and the empty positions in between count the way the zeros inside a number count

```ts
vector.leftSignificantValue(0);   // 'a'  — index 10
vector.leftSignificantValue(2);   // ''   — index 12, an empty position
vector.leftSignificantValue(3);   // 'd'  — index 13
```

Both take a signed offset, so a negative `n` walks the other way, into the default value. They throw `RangeError` only when the vector holds no entry at all, there being no position to measure from

### `SV_vector.from(elements, defaultValue?)`

Builds from an iterable of `SV_element`; entries strictly equal to the default are dropped; for a repeated index the last one wins

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

The serialization shape from the [project README](https://github.com/Calbona/sparse-vector#serialization). A faithful round trip:

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

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
