[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · **简体中文** · [繁體中文](zh-TW.md)

# sparse-vector

## 概述

- **稀疏向量的定义**

	稀疏向量不是数学上的向量，它更像你实际写数字的方式，“下标”从正无穷一直延伸到负无穷。当然，计算机的语言里没有无穷大，所以在 TypeScript 里它是 `number`，C++ 里是 `int64_t`，Rust 里是 `i64`。

	既然不是向量，自然也不能做代数运算。

- **稀疏向量的原理**

	你询问任何一个位权上的值，都能得到回应，这是怎样实现的呢？

	我们只存与默认值不同的位置，另存一个默认值。你查那些没存的位置，向量就把默认值给你。

## 库

| 语言 | 包 | 最新版本 | 状态 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | 已发布 | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | 已发布 | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | 已发布 | [`rust/`](../rust/) |

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

### 向量

- 不是指数学上的向量，也不是指计算机里的数组，而是指库提供的这种特殊数据结构。

### 稀疏

- 这就是说这个向量的容量远远大于它的元素个数，有些位权没有显式地存入数据。

### 索引

- 向量的索引是全体整数，可正可负，它表示的是类似个位、十位这样的概念。

### 值

- 我们真正想要存储的东西，类比于百位上的数字、千位上的数字，不过类型不一定是数，可以是任何东西。

### 元素

- 一个索引加一个值，组成的对象就叫元素，这些就是向量真正存储的东西。

### 默认值

- 稀疏向量没有被显式存入数据的地方，就是默认值，你可以类比于写数字时，不写的那些 0，我们一般会写 1，而不是写 0001.000，对吧？

- 构造好向量之后，默认值是可以替换的，这很神奇，不知道能用来做什么，但我给你预留了这个能力。

### 自动维持最小内存

- 替换默认值时立即清除那些值等于默认值的元素。

- 替换相等判定时也一样。

- 往某个位权写入的值如果是默认值，相当于把那里原有的东西清除。

### 相等的判定

- 值是否等于默认值？按照各语言日常的判定方法。
	- TypeScript：`===`。
	- C++：`operator==`。
	- Rust：`PartialEq`。

- 你也可以给向量一个判定：它接收两个参数——待比较的值与当前默认值——返回布尔值，取代日常的判定。

- 比较两个向量是两件具名的事，与剔除是两回事：`isEqualTo` / `is_equal_to` 判定两个向量是否相等，`differences` 列出接收者与对方不同的那些元素。两者都**以接收者的判定为准**，所以两边判定不同时，`a.isEqualTo(b)` 与 `b.isEqualTo(a)` 可以给出不同的答案。

- 注意各语言的相等判定方法有一些可能违背直觉的情形。
	- `NaN` 不等于它自己。
	- `0`、`'0'`、`false`、`null` 类型都不同，所以不相等。
	- `===` 比较的是对象的引用，而 `operator==` 与 `PartialEq` 比较的是结构。

## API

| 作用 | TypeScript | C++ | Rust | 返回类型 |
| --- | --- | --- | --- | --- |
| 构造向量（默认值缺省） | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | 新向量 |
| 构造向量 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | 新向量 |
| 访问默认值 | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts 值，cpp、rust 引用 |
| 更新默认值 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts 无，cpp、rust 布尔型 |
| 访问相等判定 | `getEquality` | `get_equality()` | `get_equality()` | ts 判定或 `undefined`，cpp、rust 判定或空 |
| 更新相等判定 | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts 无，cpp、rust 布尔型 |
| 获取元素数 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | 整数 |
| 获取有效维数 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | 整数 |
| 获取正有效维数 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | 整数 |
| 获取负有效维数 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | 整数 |
| 访问索引处的值 | `get(index)` | `get(index)` | `get(index)` | 值的类型 |
| 更新索引处的值 | `set(index, value)` | `set(index, value)` | `set(index, value)` | 向量自身 |
| 重置索引处的值 | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | 布尔型 |
| 重置整个向量，不重置默认值 | `resetVector()` | `reset_vector()` | `reset_vector()` | 布尔型 |
| 访问全部元素，按索引降序 | `elements()` | `elements()` | `elements()` | 元素数组 |
| 访问全部元素，按索引升序 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | 元素数组 |
| 访问全部索引，降序 | `indexes()` | `indexes()` | `indexes()` | 索引数组 |
| 访问全部索引，升序 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | 索引数组 |
| 访问全部值，按索引降序 | `values()` | `values()` | `values()` | 值数组 |
| 访问全部值，按索引升序 | `invertedValues()` | `inverted_values()` | `inverted_values()` | 值数组 |
| 访问高位起第 n+1 个元素 | `element(n)` | `element(n)` | `element(n)` | 元素 |
| 访问高位起第 n+1 个元素的索引 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | 索引 |
| 访问高位起第 n+1 个元素的值 | `elementValue(n)` | `element_value(n)` | `element_value(n)` | 值 |
| 访问低位起第 n+1 个元素 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | 元素 |
| 访问低位起第 n+1 个元素的索引 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | 索引 |
| 访问低位起第 n+1 个元素的值 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | 值 |
| 访问高位起第 n+1 位有效数字 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | 值 |
| 访问低位起第 n+1 位有效数字 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | 值 |
| 迭代 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | 迭代器（按索引降序借出元素） |
| 复制 | `clone()` | 拷贝构造 | `clone()` | 新向量 |
| 批量构造 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | 新向量 |
| 判定两个向量是否相等 | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | 布尔型 |
| 列出与另一个向量不同的元素 | `differences(other)` | `differences(other)` | `differences(other)` | 元素数组 |

*Rust 的取值与列举要求 `T: Clone`，写入、重置与改默认值要求 `T: PartialEq`，判定两个向量是否相等同样要求 `T: PartialEq`，求差还要求 `T: Clone`；C++ 要求可拷贝与 `operator==`*

*求差的前提是两个默认值同类型且按接收者的判定相等，不满足则 TypeScript 抛 `TypeError`，C++ 抛 `std::invalid_argument`，Rust panic*

*越界，或在空向量上取有效数字时，TypeScript 抛 `TypeError` / `RangeError`，C++ 抛 `std::out_of_range`，Rust panic*

## 许可证

MIT
