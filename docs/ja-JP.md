[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · **日本語** · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

スパースベクトル：整数の添字から任意の値への写像で、既定値と異なる位置だけを格納する

## 考え方

添字は負でもよく、連続していなくてもよい

つまり、要素が三つしかないベクトルは三つ分しか占めない —— その三つの添字が `0, 1, 2` でも `-10^9, 0, 10^9` でも

このライブラリは意図的に、数学的な意味でのベクトルでは**ない**。演算は一切持たず、ただのデータ構造である

## 実装

同じ型を三つの言語で提供している。以下に挙げる規則はすべて共通で、違いは綴りだけである

| 言語 | パッケージ | ディレクトリ | 状態 |
| --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | [`typescript/`](../typescript/) | リリース済み |
| C++ | — | [`c++/`](../c++/) | 予定 |
| Rust | `sparse-vector-rs` | [`rust/`](../rust/) | 予定 |

各実装のディレクトリには、その言語のインストール・使い方・API リファレンスを記した README がある。このページでは三つの実装に共通するセマンティクスを定義し、三度繰り返さずに済むようにしている

## すべての実装に共通するセマンティクス

### 既定値

ベクトルは既定値とともに生成される。既定値とは、明示的な要素を持たないすべての位置が返す値である。省略時は number の `0`

既定値はあとから差し替えられる。差し替えると、新しい既定値と等しいと判定される要素が即座に取り除かれる

すべての位置に値が定義されているため、読み取りは全域的である。格納されていてもいなくても、範囲内でも遠く外でも、任意の整数は例外ではなく値を返す

### 既定値に等しい要素は決して保持されない

ある位置に既定値を書き込むことは、そこにあったものを取り除くのと同じである。これがこの構造を疎に保つ：メモリは既定値と実際に異なる要素の数 k に対して O(k) であり、添字がどれだけ離れていても、どれだけ負であっても変わらない

### 取り除きは厳密等価で行う

取り除きには各言語の最も厳密な等価判定を使う —— TypeScript なら `===`、C++ なら `==`、Rust なら `PartialEq`。そこから二つの帰結が出る：

- `0`、`'0'`、`false`、`null` は四つの異なる値であり、完全に一致したものだけが取り除かれる
- `NaN` と `NaN` は厳密には等しくないので、既定値自体が `NaN` であっても、格納した `NaN` は保持される

### シリアライズ

要素は、ちょうど二つのキーを持つ普通のオブジェクトである：

| キー | 型 | 意味 |
| --- | --- | --- |
| `index` | 整数 | 位置。負でもよい |
| `value` | 任意 | そこに格納されている値 |

ベクトルは要素だけを添字の昇順でシリアライズする。既定値はこの形式には含まれないので、往復させる際は一緒に持ち回る必要がある

## TypeScript

### インストール

```sh
npm install @calbona/sparse-vector
```

### 使い方

```ts
import { SV_vector } from '@calbona/sparse-vector';

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

### API

#### `new SV_vector<T>(defaultValue?)`

ベクトルを生成する

`defaultValue` は空き位置の既定値、つまり明示的な要素がないすべての位置が返す値で、省略時は number の `0`

`T` の省略時は `number`

#### プロパティ

| メンバー | 説明 |
| --- | --- |
| `defaultValue: T` | 読み書き可能。代入すると、新しい既定値と厳密に等しい要素が即座に取り除かれる |
| `size: number` | 明示的に格納されている要素数 |

#### メソッド

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

#### `SV_vector.from(elements, defaultValue?)`

`SV_element` の反復可能オブジェクトから構築する。既定値と厳密に等しい要素は捨てられる。同じ添字が繰り返された場合は最後のものが優先される

#### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

上文で述べたシリアライズ形式である。往復させる場合はこう書く：

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

### 開発

```sh
npm run build      # dist/ にコンパイル
npm run typecheck  # src とテストの型を検査
npm test           # 先にコンパイルし、その後テスト
```

Node 24+ が必要

このライブラリ自体に実行時依存はない

## C++

まだリリースされていない。`SV_vector` と `SV_element` として提供する予定である

## Rust

まだリリースされていない。`sparse-vector-rs` クレートで `SparseVector` と `Element` として提供する予定である

## ライセンス

MIT
