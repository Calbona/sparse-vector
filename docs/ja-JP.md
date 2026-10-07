[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · **日本語** · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## 概要

- **スパースベクトルの定義**

	スパースベクトルは数学的なベクトルではない。むしろ、人が数を手で書くときの書き方に近い。「添字」は正の無限大から負の無限大まで伸びている。とはいえコンピュータの言語に無限大はないので、TypeScript では `number`、C++ では `int64_t`、Rust では `i64` になる。

	ベクトルではない以上、代数演算も持たない。

- **スパースベクトルの原理**

	どの位取りについて尋ねても、必ず値が返ってくる。これはどうやって実現しているのか。

	既定値と異なる位置だけを格納し、それとは別に既定値をひとつ持つ。何も格納していない位置を尋ねられれば、ベクトルは既定値をそのまま返す。

## ライブラリ

| 言語 | パッケージ | 最新バージョン | 状態 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | リリース済み | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | リリース済み | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | リリース済み | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*まだ vcpkg や Conan にはリリースされていない*

CMake からリポジトリを指せばよい

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

プロジェクトの隣にチェックアウトを置く場合も同じで、`add_subdirectory(path/to/sparse-vector/c++)` と書く
インストール先には CMake パッケージもエクスポートされるので、`find_package(sparse-vector)` も使える

### Rust

```sh
cargo add sparse-vector-rs
```

## セマンティクスの詳細

### ベクトル

- 数学的なベクトルや計算機の配列ではなく、このライブラリが提供する特殊なデータ構造を指す。

### 疎

- このベクトルは容量が要素数よりはるかに大きく、一部の位取りにはデータが明示的に格納されていない、ということ。

### インデックス

- ベクトルの添字は正負を問わず整数全体をとり、一の位や十の位にあたる概念を表す。

### 値

- 本当に格納したいもの。百の位や千の位の数字にたとえられるが、型は数とは限らず、何でもよい。

### 要素

- 添字ひとつと値ひとつを組にしたオブジェクトを要素と呼ぶ。ベクトルが実際に格納しているのはこれである。

### 既定値

- スパースベクトルでデータが明示的に格納されていない位置の値が既定値である。数を書くときに書き漏らす 0 のようなもので、ふつうは 0001.000 とは書かず 1 と書く。

- ベクトルを構築したあとからでも、既定値は差し替えられる。不思議な話だが、何に使えるのかは分からない。それでも、その余地は用意してある。

### 自動的に最小のメモリを保つ

- 既定値を差し替えると、それに等しい要素は即座に取り除かれる。

- 等価判定を差し替えたときも同じ。

- ある位取りに書き込む値が既定値なら、そこにあったものを取り除くのと同じ。

### 等価の判定

- ある値が既定値と等しいかどうかは、各言語が備える通常の判定に従う。
	- TypeScript：`===`。
	- C++：`operator==`。
	- Rust：`PartialEq`。

- ベクトルに判定を渡すこともできる。これは比較する値と現在の既定値の二つを受け取り、ブール値を返すもので、通常の判定に取って代わる。

- 二つのベクトルを比べることは、名前を持った二つの操作であって、取り除きとは別である。`isEqualTo` / `is_equal_to` は二つのベクトルが等しいかどうかを判定し、`differences` はレシーバと相手で異なる要素を列挙する。どちらも**レシーバの判定に従う**ので、両者の判定が異なるときには `a.isEqualTo(b)` と `b.isEqualTo(a)` が違う答えを返すことがある。

- 各言語の等価判定には、直感に反する場合がある。
	- `NaN` はそれ自身と等しくない。
	- `0`、`'0'`、`false`、`null` は型がすべて異なるので等しくない。
	- `===` はオブジェクトの参照を比較し、`operator==` と `PartialEq` は構造を比較する。

## API

| 役割 | TypeScript | C++ | Rust | 戻り値の型 |
| --- | --- | --- | --- | --- |
| スパースベクトルの生成（既定値は省略可） | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | 新しいベクトル |
| スパースベクトルの生成 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | 新しいベクトル |
| 既定値の取得 | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts は値、cpp、rust は参照 |
| 既定値の変更 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts はなし、cpp、rust はブール型 |
| 等価判定の取得 | `getEquality` | `get_equality()` | `get_equality()` | ts は判定または `undefined`、cpp、rust は判定または空 |
| 等価判定の変更 | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts はなし、cpp、rust はブール型 |
| 要素数の取得 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | 整数 |
| 有効次元の取得 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | 整数 |
| 正の有効次元の取得 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | 整数 |
| 負の有効次元の取得 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | 整数 |
| 添字の位置の値を取得 | `get(index)` | `get(index)` | `get(index)` | 値の型 |
| 添字の位置に値を書き込む | `set(index, value)` | `set(index, value)` | `set(index, value)` | ベクトル自身 |
| 添字の位置の値をリセット | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | ブール型 |
| ベクトル全体をリセット（既定値はリセットしない） | `resetVector()` | `reset_vector()` | `reset_vector()` | ブール型 |
| すべての要素を添字の降順で取得 | `elements()` | `elements()` | `elements()` | 要素の配列 |
| すべての要素を添字の昇順で取得 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | 要素の配列 |
| 空でない添字をすべて降順で取得 | `indexes()` | `indexes()` | `indexes()` | 添字の配列 |
| 空でない添字をすべて昇順で取得 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | 添字の配列 |
| 空でない値をすべて降順で取得 | `values()` | `values()` | `values()` | 値の配列 |
| 空でない値をすべて昇順で取得 | `invertedValues()` | `inverted_values()` | `inverted_values()` | 値の配列 |
| 左から n+1 番目の要素を取得 | `element(n)` | `element(n)` | `element(n)` | 要素 |
| その要素の添字を取得 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | 添字 |
| その要素の値を取得 | `elementValue(n)` | `element_value(n)` | `element_value(n)` | 値 |
| 右から n+1 番目の要素を取得 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | 要素 |
| その要素の添字を取得 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | 添字 |
| その要素の値を取得 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | 値 |
| 左から n+1 番目の有効数字を取得 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | 値 |
| 右から n+1 番目の有効数字を取得 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | 値 |
| 反復 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | イテレータ（添字の降順で要素を貸し出す） |
| 複製 | `clone()` | コピー構築 | `clone()` | 新しいベクトル |
| 一括生成 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | 新しいベクトル |
| 二つのベクトルが等しいかどうかを判定 | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | ブール型 |
| 別のベクトルと異なる要素を列挙 | `differences(other)` | `differences(other)` | `differences(other)` | 要素の配列 |

*Rust では値の取得と列挙に `T: Clone`、書き込み・リセット・既定値の変更に `T: PartialEq` が必要。二つのベクトルが等しいかどうかの判定にも `T: PartialEq`、差分の計算にはさらに `T: Clone` が必要。C++ ではコピー可能であることと `operator==` が必要*

*差分を求めるには、二つの既定値が同じ型で、かつレシーバの判定のもとで等しいことが前提となる。満たさない場合、TypeScript は `TypeError` を投げ、C++ は `std::invalid_argument` を投げ、Rust は panic する*

*範囲外、または空のベクトルで有効数字を取ろうとしたとき、TypeScript は `TypeError` / `RangeError` を投げ、C++ は `std::out_of_range` を投げ、Rust は panic する*

## ライセンス

MIT
