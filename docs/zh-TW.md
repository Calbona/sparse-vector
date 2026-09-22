[English](../README.md) · [简体中文](zh-CN.md) · **繁體中文** · [日本語](ja-JP.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

稀疏向量：一個從整數下標到任意值的映射，只儲存與空位預設值不同的位置

## 概念

下標可以為負，也不必連續

所以一個只有三個條目的向量就只佔三個條目 —— 無論這三個下標是 `0, 1, 2` 還是 `-10^9, 0, 10^9`

本函式庫刻意**不是**數學意義上的向量。它不帶任何算術；它就是一種資料結構，僅此而已

## 實作

同一個型別以三種語言提供。它們共用以下每一條規則 —— 差別只在寫法

| 語言 | 套件 | 版本 | 目錄 | 狀態 |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 1.1.0 | [`typescript/`](../typescript/) | 已發布 |
| C++ | `sparse-vector` | 1.1.0 | [`c++/`](../c++/) | 已發布 |
| Rust | `sparse-vector-rs` | 1.1.0 | [`rust/`](../rust/) | 已發布 |

三個套件各自獨立編號，所以版本號不同。目前三者實作的語意是一致的

## 安裝

**TypeScript**

```sh
npm install @calbona/sparse-vector
```

**C++** —— 尚未發佈到 vcpkg 或 Conan。只有兩個標頭檔、無需連結，讓 CMake 指向倉庫即可：

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

放在專案旁邊的檢出目錄同樣可行，寫 `add_subdirectory(path/to/sparse-vector/c++)`。安裝到前綴後也會匯出 CMake 套件，因此 `find_package(sparse-vector)` 也可用

**Rust** —— 注意匯入的 crate 名是 `sparse_vector`，不是套件名：

```sh
cargo add sparse-vector-rs
```

每個實作目錄都各自帶有該語言的用法與 API 參考的說明文件。本頁定義它們共通的語意，因此不必重複寫三次

## 各實作共通的語意

### 空位預設值

向量建立時帶有一個預設值：所有沒有顯式條目的位置所回報的值。未指定時預設為 number 型別的 `0`

預設值之後可以替換。替換時會立即排除所有等於新預設值的條目

因為每個位置都有定義好的值，所以讀取是全定義的。任何整數 —— 已儲存或未儲存、在範圍內或遠在範圍外 —— 都會回傳一個值，而不是拋錯

### 等於空位預設值的條目永遠不會被保留

把預設值寫進某個位置，就等同於移除原本在那裡的東西。這正是讓結構保持稀疏的原因：記憶體是 O(k)，k 為真正不同於預設值的條目數，無論下標相隔多遠、負到什麼程度

### 排除採用各語言自己的相等判斷

當某個條目與新的預設值比較為相等時就會被排除，用的是該語言平日的相等 —— TypeScript 用 `===`，C++ 用 `operator==`，Rust 用 `PartialEq`

對數字與字串，三者完全一致，包括那些彆扭的情況：

- `-0.0` 等於 `0.0`，所以預設值為 `0.0` 時，存進去的 `-0.0` 會被排除
- `NaN` 不等於它自己，所以即使預設值本身就是 `NaN`，存進去的 `NaN` 依然會被保留

TypeScript 的 `0`、`'0'`、`false`、`null` 是四個不同型別的值，只有完全相符才會排除該條目。靜態型別的向量只持有單一的 `T`，所以在 C++ 與 Rust 裡這組值根本不會出現 —— 但它所說明的規則「相等是精確的，不做隱式轉換」在三者中都成立

對物件而言三者才真正分道揚鑣，而這是值的「同一性」唯一可見的地方。TypeScript 的 `===` 比較的是物件的參考，`operator==` 與 `PartialEq` 通常比較的是結構。兩個內容相同但彼此獨立的物件，在 TypeScript 裡是一個值，在 C++ 與 Rust 裡是兩個值 —— 所以內容相同但彼此獨立的條目會被前者保留、被後兩者排除。需要同一性時，就把它做進型別自身的相等裡 —— 用指標型別可以直接得到：`std::shared_ptr` 的 `operator==` 比較的是指標，`Rc<T>` 則可以用一個以 `Rc::ptr_eq` 比較的 newtype 包起來。C++ 與 Rust 的 README 各自給了這個寫法

### 序列化

條目是一個恰好含兩個鍵的普通物件：

| 鍵 | 型別 | 意義 |
| --- | --- | --- |
| `index` | 整數 | 位置，可負可不負 |
| `value` | 任意 | 存放在該處的值 |

向量序列化後就只有它的條目，並按下標升冪排列。預設值不屬於這個結構，因此往返時要把它一併帶上

## TypeScript

已發布為 `@calbona/sparse-vector`。用法與 API 參考見 [`typescript/README.md`](../typescript/README.md)

## C++

純標頭檔的 C++17 實作，從倉庫接入，尚未進入 vcpkg 或 Conan。用法與 API 參考見 [`c++/README.md`](../c++/README.md)

## Rust

已發佈為 `sparse-vector-rs`。用法與 API 參考見 [`rust/README.md`](../rust/README.md)

## 授權條款

MIT
