[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · **日本語** · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

TypeScript のスパースベクトル：整数の添字から任意の値への写像で、空き位置の既定値と異なる位置だけを格納する

添字は負でもよく、連続していなくてもよい

つまり、要素が三つしかないベクトルは三つ分しか占めない —— その三つの添字が `0, 1, 2` でも `-10^9, 0, 10^9` でも

このライブラリは意図的に、数学的な意味でのベクトルでは**ない**

## インストール

```sh
npm install sparse-vector
```

## 使い方

```ts
import { SV_vector } from 'sparse-vector';

const vector = new SV_vector(); // 空き位置の既定値：number の 0

vector.set(1_000_000, '遠い');
vector.set(-42, '負');

vector.get(1_000_000); // '遠い'
vector.get(-42);       // '負'
vector.get(7);         // 0
vector.size;           // 2
```

空き位置の既定値は指定でき、あとから変更もできる。明示的な要素がない位置はすべてこの値として読める

```ts
const counts = new SV_vector<number | null>(null); // 空き位置は null
counts.set(3, 1);
counts.get(4); // null

counts.defaultValue = 0; // 空き位置が 0 になる
```

値の型は制限されない。何でもよい

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

## API

### `new SV_vector<T>(defaultValue?)`

ベクトルを生成する

`defaultValue` は空き位置の既定値、つまり明示的な要素がないすべての位置が返す値で、省略時は number の `0`

`T` の省略時は `number`

### プロパティ

| メンバー | 説明 |
| --- | --- |
| `defaultValue: T` | 読み書き可能。代入すると、新しい既定値と厳密に等しい要素が即座に取り除かれる |
| `size: number` | 明示的に格納されている要素数 |

### メソッド

| メソッド | 説明 |
| --- | --- |
| `get(index): T` | `index` の値。そこに明示的な要素がなければ `defaultValue` を返す |
| `set(index, value): this` | 挿入または更新。チェーンできる |
| `has(index): boolean` | `index` に明示的な要素があるか |
| `delete(index): boolean` | そこの明示的な要素を削除し、既定値に戻す |
| `clear(): void` | すべての明示的な要素を削除し、既定値に戻す |
| `elements(): SV_element<T>[]` | 明示的な要素を添字の昇順で |
| `keys(): number[]` | 格納されている添字を昇順で |
| `values(): T[]` | 格納されている値を添字の昇順で |
| `clone(): SV_vector<T>` | 複製 |
| `[Symbol.iterator]()` | 明示的な要素を添字の昇順で反復する |
| `toJSON(): SV_element<T>[]` | `elements()` と同じ。したがって `JSON.stringify` がそのまま使える |

`index` は整数でなければならない —— 正でも負でもよい。整数でなければ `TypeError` を投げる

データの範囲外の位置を読んでも例外にはならず既定値を返す。これがこの型の存在意義である

### `SV_vector.from(elements, defaultValue?)`

`SV_element` の反復可能オブジェクトから構築する。既定値と厳密に等しい要素は捨てられる。同じ添字が繰り返された場合は最後のものが優先される

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

これはシリアライズ形式である：ちょうど `index` と `value` の二つのキーを持つ普通の JSON オブジェクト

`toJSON()` は要素だけを出力し、既定値はこの構造には含まれない。したがって JSON を往復させる際は既定値も一緒に持ち回る必要がある

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

## 取り除きの規則

空き位置の既定値に等しい要素は決して保持されない

判定は**厳密等価**で行う。そこから二つの帰結が出る：

- `0`、`'0'`、`false`、`null` は四つの異なる値であり、`===` が成り立つものだけが取り除かれる
- `NaN` と `NaN` は厳密には等しくないので、既定値自体が `NaN` であっても、格納した `NaN` は保持される

## 開発

```sh
npm run build      # dist/ にコンパイル
npm run typecheck  # src とテストの型を検査
npm test           # 先にコンパイルし、その後テスト
```

Node 24+ が必要

このライブラリ自体に実行時依存はない

## License

MIT
