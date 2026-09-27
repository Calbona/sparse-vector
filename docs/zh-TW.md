[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · **繁體中文**

# sparse-vector

## 概述

- **稀疏向量不是向量**

	它的「下標」既不從零開始，也不從左往右遞增，而更像人實際寫數字的方式：下標相當於位權，因此它能從正無窮一路延伸到負無窮。當然，語言裡並沒有無窮大，在 TypeScript 裡它是 `number`，C++ 裡是 `int64_t`，Rust 裡是 `i64`。

	既然不是數學意義上的向量，它自然也不帶任何運算。從這個角度看，它其實也算一種字典，而且每個位權上確實都能存任意型別的資料。

- **稀疏向量的實作原理**

	我們只存那些與預設值不同的位置。

	把若干個叫「元素」的物件——每個是一個索引配上一個不同於預設值的值——排成一個陣列，再加上預設值，噔噔，一個稀疏向量就完成了！無論下標落在哪裡，只要只有 k 個元素，記憶體就是 O(k)。

## 函式庫

| 語言 | 套件 | 最新版本 | 狀態 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | 已發布 | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | 已發布 | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | 已發布 | [`rust/`](../rust/) |

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

### 空位預設值

- 建立稀疏向量時指定一個預設值

- 預設值之後可以替換

- 每個位權上都有確定的值，因此讀取總有答案：任何整數都會回傳一個值

### 等於預設值的條目永遠不會被保留

- 往某個位權寫入預設值，等同於把那裡原有的東西擦除

- 替換預設值時會立即剔除那些等於預設值的條目

- 正是這個原則讓結構保持稀疏

### 相等的判定

- 條目是否等於預設值，依各語言日常的判定：TypeScript 用 `===`，C++ 用 `operator==`，Rust 用 `PartialEq`

- 那些彆扭的情形：
	- `-0.0` 等於 `0.0`
	- `NaN` 不等於它自己
	- `0`、`'0'`、`false`、`null` 是四個不同型別的值
	- `===` 比較的是物件的參考，而 `operator==` 與 `PartialEq` 比較的是結構，兩個內容相同但彼此獨立的物件，在 TypeScript 裡是一個值，在 C++ 和 Rust 裡是兩個值，所以前者保留、後兩者剔除

- 需要同一性時，就把它做進型別自身的相等裡，用指標型別可以直接得到：`std::shared_ptr` 的 `operator==` 比較的是指標，`Rc<T>` 則可以用一個以 `Rc::ptr_eq` 比較的 newtype 包起來，具體見 C++ 與 Rust 的 README

## API

| 作用 | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| 建構稀疏向量（預設值缺省） | `new SV_vector()` | `SV_vector()` | `SparseVector::new()`（僅 `f64`）/ `SparseVector::default()` |
| 建構稀疏向量 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| 取得預設值 | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| 修改預設值 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| 取得條目數 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| 取得有效維數 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| 取得正有效維數 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| 取得負有效維數 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| 取得索引處的值 | `get(index)` | `get(index)` | `get(index)` |
| 寫入索引處的值 | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| 重置索引處的值 | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| 重置整個向量 | `resetVector()` | `reset_vector()` | `reset_vector()` |
| 取得全部條目，按索引降序 | `elements()` | `elements()` | `elements()` |
| 取得全部條目，按索引升序 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| 取得全部非空索引，按索引降序 | `indexes()` | `indexes()` | `indexes()` |
| 取得全部非空索引，按索引升序 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| 取得全部非空值，按索引降序 | `values()` | `values()` | `values()` |
| 取得全部非空值，按索引升序 | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| 取得從左數第 n+1 個條目 | `element(n)` | `element(n)` | `element(n)` |
| 取得從左數第 n+1 個條目的索引 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| 取得從左數第 n+1 個條目的值 | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| 取得從右數第 n+1 個條目 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| 取得從右數第 n+1 個條目的索引 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| 取得從右數第 n+1 個條目的值 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| 取得左起第 n+1 位有效數字 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| 取得右起第 n+1 位有效數字 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| 迭代 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| 複製 | `clone()` | 拷貝建構 | `clone()` |
| 批次建構 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*Rust 的取值與列舉要求 `T: Clone`，寫入、重置與改預設值要求 `T: PartialEq`；C++ 要求可複製與 `operator==`*

*越界，或在空向量上取有效數字時，TypeScript 拋 `TypeError` / `RangeError`，C++ 拋 `std::out_of_range`，Rust panic*

## 授權條款

MIT
