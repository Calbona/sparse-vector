[English](../README.md) · **简体中文** · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

稀疏向量：一个从整数下标到任意值的映射，只存储与默认值不同的位置

## 概念

下标可以为负，也不必连续

所以一个只有三个条目的向量就只占三个条目 —— 无论这三个下标是 `0, 1, 2` 还是 `-10^9, 0, 10^9`

本库刻意**不是**数学意义上的向量。它不带任何运算，它就是一个数据结构，仅此而已

## 实现

同一个类型在三种语言中提供，以下每一条规则它们都共有 —— 只是写法不同

| 语言 | 包 | 版本 | 目录 | 状态 |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 1.1.0 | [`typescript/`](../typescript/) | 已发布 |
| C++ | `sparse-vector` | 1.1.0 | [`c++/`](../c++/) | 已发布 |
| Rust | `sparse-vector-rs` | 1.1.0 | [`rust/`](../rust/) | 已发布 |

三个包各自独立编号，所以版本号不同。目前三者实现的语义是一致的

## 安装

**TypeScript**

```sh
npm install @calbona/sparse-vector
```

**C++** —— 尚未发布到 vcpkg 或 Conan。只有两个头文件、无需链接，让 CMake 指向仓库即可：

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

放在项目旁边的检出目录同样可行，写 `add_subdirectory(path/to/sparse-vector/c++)`。安装到前缀后也会导出 CMake 包，因此 `find_package(sparse-vector)` 也可用

**Rust** —— 注意导入的 crate 名是 `sparse_vector`，不是包名：

```sh
cargo add sparse-vector-rs
```

每个实现目录都有自己的 README，里面有该语言的用法与 API 参考。本页定义它们共有的语义，这样就不必重复写三遍

## 各实现共有的语义

### 空位默认值

创建向量时会指定一个默认值：所有没有显式条目的位置所报告的值。除非另行给定，它缺省为 number 类型的 `0`

默认值之后可以替换。替换时会立即剔除所有与新默认值相等的条目

因为每个位置都有确定的值，取值总是有结果的。任何整数 —— 无论是否存储过、在范围内还是远在范围之外 —— 都会返回一个值，而不会抛错

### 等于默认值的条目永远不被保留

往某个位置写入默认值，等同于把那里原有的东西删掉。正是这一点让结构保持稀疏：内存为 O(k)，k 是真正与默认值不同的条目数 —— 无论这些下标彼此相隔多远，或者负到什么程度

### 剔除采用各语言自己的相等判定

一条条目与新默认值比较相等时会被剔除，用的是该语言日常的相等 —— TypeScript 用 `===`，C++ 用 `operator==`，Rust 用 `PartialEq`

对数字和字符串，三者完全一致，包括那些别扭的情形：

- `-0.0` 等于 `0.0`，所以默认值为 `0.0` 时，存进去的 `-0.0` 会被剔除
- `NaN` 不等于它自己，所以即使默认值本身就是 `NaN`，存进去的 `NaN` 依然会被保留

TypeScript 的 `0`、`'0'`、`false`、`null` 是四个不同类型的值，只有完全相等才会剔除一条条目。静态类型的向量只持有单一的 `T`，所以在 C++ 和 Rust 里这一组值根本不会出现 —— 但它说明的规则「相等是精确的，不做隐式转换」在三者中都成立

对对象而言三者才真正分道扬镳，而这是值的「同一性」唯一可见的地方。TypeScript 的 `===` 比较的是对象的引用，`operator==` 与 `PartialEq` 通常比较的是结构。两个内容相同但彼此独立的对象，在 TypeScript 里是一个值，在 C++ 和 Rust 里是两个值 —— 所以一条内容相同但彼此独立的条目会被前者保留、被后两者剔除。需要同一性时，就把它做进类型自身的相等里 —— 用指针类型可以直接得到：`std::shared_ptr` 的 `operator==` 比较的是指针，`Rc<T>` 则可以用一个以 `Rc::ptr_eq` 比较的 newtype 包起来。C++ 与 Rust 的 README 各自给了这个写法

### 序列化

一个条目是一个恰好含两个键的普通对象：

| 键 | 类型 | 含义 |
| --- | --- | --- |
| `index` | 整数 | 位置，可负可不负 |
| `value` | 任意 | 存储在该处的值 |

向量的序列化结果只有条目本身，按下标升序。默认值不在这套结构里，因此往返时要把默认值一并带上

## TypeScript

已发布为 `@calbona/sparse-vector`。用法与 API 参考见 [`typescript/README.md`](../typescript/README.md)

## C++

纯头文件的 C++17 实现，从仓库接入，尚未进入 vcpkg 或 Conan。用法与 API 参考见 [`c++/README.md`](../c++/README.md)

## Rust

已发布为 `sparse-vector-rs`。用法与 API 参考见 [`rust/README.md`](../rust/README.md)

## 许可证

MIT
