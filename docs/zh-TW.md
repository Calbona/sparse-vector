[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · **繁體中文**

# sparse-vector

## 概述

- **稀疏向量不是向量**

	它不是數學上的向量，而更像人實際寫數字的方式：「下標」從正無窮一路延伸到負無窮。當然，語言裡並沒有無窮大，所以在 TypeScript 裡它是 `number`，C++ 裡是 `int64_t`，Rust 裡是 `i64`。

	既然不是向量，自然也不帶任何代數運算。

- **稀疏向量的實作原理**

	你詢問任何一個位權上的值，都能得到回應，這是怎樣實現的呢？

	我們只存與預設值不同的位置，另存一個預設值。你查那些沒存的位置，向量就把預設值給你。

## 函式庫

| 語言 | 套件 | 最新版本 | 狀態 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | 已發布 | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | 已發布 | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | 已發布 | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*尚未發布到 vcpkg 或 Conan*

讓 CMake 指向倉庫即可

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

放在專案旁邊的檢出目錄同樣可行，寫 `add_subdirectory(path/to/sparse-vector/c++)`
安裝到前綴後也會匯出 CMake 套件，因此 `find_package(sparse-vector)` 也可用

### Rust

```sh
cargo add sparse-vector-rs
```

## 語意詳解

### 向量

- 不是指數學上的向量，也不是指電腦裡的陣列，而是指本函式庫提供的這種特殊資料結構。

### 稀疏

- 這就是說這個向量的容量遠遠大於它的元素個數，有些位權沒有顯式地存入資料。

### 索引

- 向量的索引是全體整數，可正可負，它表示的是類似個位、十位這樣的概念。

### 值

- 我們真正想要儲存的東西，類比於百位上的數字、千位上的數字，不過型別不一定是數，可以是任何東西。

### 元素

- 一個索引加一個值，組成的物件就叫元素，這些就是向量真正儲存的東西。

### 預設值

- 稀疏向量沒有被顯式存入資料的地方，就是預設值，你可以類比於寫數字時，不寫的那些 0，我們一般會寫 1，而不是寫 0001.000，對吧？

- 建構好向量之後，預設值是可以替換的，這很神奇，不知道能用來做什麼，但我給你預留了這個能力。

### 自動維持最小記憶體

- 替換預設值時立即清除那些值等於預設值的元素。

- 替換相等判定時也一樣。

- 往某個位權寫入的值如果是預設值，相當於把那裡原有的東西清除。

### 相等的判定

- 值是否等於預設值？按照各語言日常的判定方法。
	- TypeScript：`===`。
	- C++：`operator==`。
	- Rust：`PartialEq`。

- 你也可以給向量一個判定：它接收兩個參數——待比較的值與當前預設值——回傳布林值，取代日常的判定。

- 比較兩個向量是兩件具名的事，與剔除是兩回事：`isEqualTo` / `is_equal_to` 判定兩個向量是否相等，`differences` 列出接收者與對方不同的那些元素。兩者都**以接收者的判定為準**，所以兩邊判定不同時，`a.isEqualTo(b)` 與 `b.isEqualTo(a)` 可以給出不同的答案。

- 注意各語言的相等判定方法有一些可能違背直覺的情形。
	- `NaN` 不等於它自己。
	- `0`、`'0'`、`false`、`null` 型別都不同，所以不相等。
	- `===` 比較的是物件的參考，而 `operator==` 與 `PartialEq` 比較的是結構。

## API

| 作用 | TypeScript | C++ | Rust | 回傳型別 |
| --- | --- | --- | --- | --- |
| 建構稀疏向量（預設值缺省） | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | 新向量 |
| 建構稀疏向量 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | 新向量 |
| 取得預設值 | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts 值，cpp、rust 參考 |
| 修改預設值 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts 無，cpp、rust 布林值 |
| 取得相等判定 | `getEquality` | `get_equality()` | `get_equality()` | ts 判定或 `undefined`，cpp、rust 判定或空 |
| 修改相等判定 | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts 無，cpp、rust 布林值 |
| 取得元素數 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | 整數 |
| 取得有效維數 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | 整數 |
| 取得正有效維數 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | 整數 |
| 取得負有效維數 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | 整數 |
| 取得索引處的值 | `get(index)` | `get(index)` | `get(index)` | 值的型別 |
| 寫入索引處的值 | `set(index, value)` | `set(index, value)` | `set(index, value)` | 向量自身 |
| 重置索引處的值 | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | 布林值 |
| 重置整個向量，不重置預設值 | `resetVector()` | `reset_vector()` | `reset_vector()` | 布林值 |
| 取得全部元素，按索引降序 | `elements()` | `elements()` | `elements()` | 元素陣列 |
| 取得全部元素，按索引升序 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | 元素陣列 |
| 取得全部索引，降序 | `indexes()` | `indexes()` | `indexes()` | 索引陣列 |
| 取得全部索引，升序 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | 索引陣列 |
| 取得全部值，按索引降序 | `values()` | `values()` | `values()` | 值陣列 |
| 取得全部值，按索引升序 | `invertedValues()` | `inverted_values()` | `inverted_values()` | 值陣列 |
| 取得從左數第 n+1 個元素 | `element(n)` | `element(n)` | `element(n)` | 元素 |
| 取得從左數第 n+1 個元素的索引 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | 索引 |
| 取得從左數第 n+1 個元素的值 | `elementValue(n)` | `element_value(n)` | `element_value(n)` | 值 |
| 取得從右數第 n+1 個元素 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | 元素 |
| 取得從右數第 n+1 個元素的索引 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | 索引 |
| 取得從右數第 n+1 個元素的值 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | 值 |
| 取得左起第 n+1 位有效數字 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | 值 |
| 取得右起第 n+1 位有效數字 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | 值 |
| 迭代 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | 迭代器（按索引降序借出元素） |
| 複製 | `clone()` | 拷貝建構 | `clone()` | 新向量 |
| 批次建構 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | 新向量 |
| 判定兩個向量是否相等 | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | 布林值 |
| 列出與另一個向量不同的元素 | `differences(other)` | `differences(other)` | `differences(other)` | 元素陣列 |

*Rust 的取值與列舉要求 `T: Clone`，寫入、重置與改預設值要求 `T: PartialEq`，判定兩個向量是否相等同樣要求 `T: PartialEq`，求差還要求 `T: Clone`；C++ 要求可複製與 `operator==`*

*求差的前提是兩個預設值同型別且按接收者的判定相等，不滿足則 TypeScript 拋 `TypeError`，C++ 拋 `std::invalid_argument`，Rust panic*

*越界，或在空向量上取有效數字時，TypeScript 拋 `TypeError` / `RangeError`，C++ 拋 `std::out_of_range`，Rust panic*

## 授權條款

MIT
