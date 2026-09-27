[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · **简体中文** · [繁體中文](zh-TW.md)

# sparse-vector

## 概述

- **稀疏向量不是向量**

	它的“下标”既不从零开始，也不从左往右递增，而更像人实际写数字的方式：下标相当于位权，因此它能从正无穷一直延伸到负无穷。当然，语言里并没有无穷大，在 TypeScript 里它是 `number`，C++ 里是 `int64_t`，Rust 里是 `i64`。

	既然不是数学意义上的向量，它自然也不带任何运算。从这个角度看，它其实也算一种字典，而且每个位权上确实都能存任意类型的数据。

- **稀疏向量的实现原理**

	我们只存那些与默认值不同的位置。

	把若干个叫“元素”的对象——每个是一个索引配上一个不同于默认值的值——排成一个列表，再加上默认值，噔噔，一个稀疏向量就完成了！不管下标落在哪里，只要只有 k 个元素，内存就是 O(k)。

## 库

| 语言 | 包 | 最新版本 | 状态 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | 已发布 | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | 已发布 | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | 已发布 | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*尚未发布到 vcpkg 或 Conan*

让 CMake 指向仓库即可

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

放在项目旁边的检出目录同样可行，写 `add_subdirectory(path/to/sparse-vector/c++)`
安装到前缀后也会导出 CMake 包，因此 `find_package(sparse-vector)` 也可用

### Rust

```sh
cargo add sparse-vector-rs
```

## 语义详解

### 空位默认值

- 创建稀疏向量时指定一个默认值

- 默认值之后可以替换

- 每个位权上都有确定的值，因此读取总有答案：任何整数都会返回一个值

### 等于默认值的条目永远不被保留

- 往某个位权写入默认值，等同于把那里原有的东西擦除

- 替换默认值时会立即剔除那些等于默认值的条目

- 正是这个原则让结构保持稀疏

### 相等的判定

- 条目是否等于默认值，照各语言日常的判定来：TypeScript 用 `===`，C++ 用 `operator==`，Rust 用 `PartialEq`

- 那些别扭的情形：
	- `-0.0` 等于 `0.0`
	- `NaN` 不等于它自己
	- `0`、`'0'`、`false`、`null` 是四个不同类型的值
	- `===` 比较的是对象的引用，而 `operator==` 与 `PartialEq` 比较的是结构，两个内容相同但彼此独立的对象，在 TypeScript 里是一个值，在 C++ 和 Rust 里是两个值，所以前者保留、后两者剔除

- 需要同一性时，就把它做进类型自身的相等里，用指针类型可以直接得到：`std::shared_ptr` 的 `operator==` 比较的是指针，`Rc<T>` 则可以用一个以 `Rc::ptr_eq` 比较的 newtype 包起来，具体见 C++ 与 Rust 的 README

## API

| 作用 | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| 构造稀疏向量（默认值缺省） | `new SV_vector()` | `SV_vector()` | `SparseVector::new()`（仅 `f64`）/ `SparseVector::default()` |
| 构造稀疏向量 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| 获取默认值 | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| 修改默认值 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| 获取条目数 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| 获取有效维数 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| 获取正有效维数 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| 获取负有效维数 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| 获取索引处的值 | `get(index)` | `get(index)` | `get(index)` |
| 写入索引处的值 | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| 重置索引处的值 | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| 重置整个向量 | `resetVector()` | `reset_vector()` | `reset_vector()` |
| 获取全部条目，按索引降序 | `elements()` | `elements()` | `elements()` |
| 获取全部条目，按索引升序 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| 获取全部非空索引，按索引降序 | `indexes()` | `indexes()` | `indexes()` |
| 获取全部非空索引，按索引升序 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| 获取全部非空值，按索引降序 | `values()` | `values()` | `values()` |
| 获取全部非空值，按索引升序 | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| 获取从左数第 n+1 个条目 | `element(n)` | `element(n)` | `element(n)` |
| 获取从左数第 n+1 个条目的索引 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| 获取从左数第 n+1 个条目的值 | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| 获取从右数第 n+1 个条目 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| 获取从右数第 n+1 个条目的索引 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| 获取从右数第 n+1 个条目的值 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| 获取左起第 n+1 位有效数字 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| 获取右起第 n+1 位有效数字 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| 迭代 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| 复制 | `clone()` | 拷贝构造 | `clone()` |
| 批量构造 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*Rust 的取值与列举要求 `T: Clone`，写入、重置与改默认值要求 `T: PartialEq`；C++ 要求可拷贝与 `operator==`*

*越界，或在空向量上取有效数字时，TypeScript 抛 `TypeError` / `RangeError`，C++ 抛 `std::out_of_range`，Rust panic*

## 许可证

MIT
