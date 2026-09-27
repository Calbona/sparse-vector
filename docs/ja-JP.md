[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · **日本語** · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## 概要

- **スパースベクトルはベクトルではない**

	その「添字」はゼロから始まるものでも、左から右へ並ぶものでもない。むしろ人が数を手書きするときの書き方に近く、添字は位取りの重みにあたるので、正の無限大から負の無限大まで広がる。もちろん言語の制約上、無限大は実在しない。TypeScript では `number`、C++ では `int64_t`、Rust では `i64` である。

	また、数学的なベクトルではない以上、演算も持たない。その意味では辞書の一種でもあり、しかもどの位にも任意の型のデータを実際に置ける。

- **スパースベクトルの仕組み**

	既定値と異なる位置だけを格納する。

	「要素」とは、添字ひとつと、既定値とは異なる値ひとつを組にしたオブジェクトのこと。それをいくつか並べて配列にし、既定値を添えればできあがり。添字が何であれ、要素が k 個しかなければメモリは O(k) である。

## ライブラリ

| 言語 | パッケージ | 最新バージョン | 状態 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | リリース済み | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | リリース済み | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | リリース済み | [`rust/`](../rust/) |

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

### 空き位置の既定値

- スパースベクトルの生成時に既定値を指定する

- 既定値はあとから差し替えられる

- どの位にも値が定まっており、読み取りは常に結果を返す。どんな整数でも値が返る

### 既定値に等しい要素は決して保持されない

- ある位に既定値を書き込むことは、そこにあったものを消すのと同じである

- 既定値を差し替えると、それに等しい要素は即座に取り除かれる

- この原則が構造を疎に保つ

### 等価判定

- ある要素が既定値と等しいかどうかは、各言語の日常的な判定に従う。TypeScript なら `===`、C++ なら `operator==`、Rust なら `PartialEq`

- 厄介な場合：
	- `-0.0` は `0.0` と等しい
	- `NaN` はそれ自身と等しくない
	- `0`、`'0'`、`false`、`null` は四つの異なる型の値である
	- `===` はオブジェクトの参照を比較し、`operator==` と `PartialEq` は構造を比較する。内容が同じで互いに独立した二つのオブジェクトは、TypeScript では一つの値、C++ と Rust では二つの値なので、前者では保持され、後二者では取り除かれる

- 同一性が必要なら、それを型自身の等価判定に組み込めばよい。ポインタ型を使えばそのまま得られる —— `std::shared_ptr` の `operator==` はポインタを比較し、`Rc<T>` は `Rc::ptr_eq` で比較する newtype で包めばよい。詳しくは C++ と Rust の README を参照

## API

| 役割 | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| スパースベクトルの生成（既定値は省略可） | `new SV_vector()` | `SV_vector()` | `SparseVector::new()`（`f64` のみ）/ `SparseVector::default()` |
| スパースベクトルの生成 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| 既定値の取得 | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| 既定値の変更 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| 要素数の取得 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| 有効次元の取得 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| 正の有効次元の取得 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| 負の有効次元の取得 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| 添字の位置の値を取得 | `get(index)` | `get(index)` | `get(index)` |
| 添字の位置に値を書き込む | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| 添字の位置の値をリセット | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| ベクトル全体をリセット | `resetVector()` | `reset_vector()` | `reset_vector()` |
| すべての要素を添字の降順で取得 | `elements()` | `elements()` | `elements()` |
| すべての要素を添字の昇順で取得 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| 空でない添字をすべて降順で取得 | `indexes()` | `indexes()` | `indexes()` |
| 空でない添字をすべて昇順で取得 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| 空でない値をすべて降順で取得 | `values()` | `values()` | `values()` |
| 空でない値をすべて昇順で取得 | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| 左から n+1 番目の要素を取得 | `element(n)` | `element(n)` | `element(n)` |
| その要素の添字を取得 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| その要素の値を取得 | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| 右から n+1 番目の要素を取得 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| その要素の添字を取得 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| その要素の値を取得 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| 左から n+1 番目の有効数字を取得 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| 右から n+1 番目の有効数字を取得 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| 反復 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| 複製 | `clone()` | コピー構築 | `clone()` |
| 一括生成 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*Rust では値の取得と列挙に `T: Clone`、書き込み・リセット・既定値の変更に `T: PartialEq` が必要。C++ ではコピー可能であることと `operator==` が必要*

*範囲外、または空のベクトルで有効数字を取ろうとしたとき、TypeScript は `TypeError` / `RangeError` を投げ、C++ は `std::out_of_range` を投げ、Rust は panic する*

## ライセンス

MIT
