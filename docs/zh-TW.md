[English](../README.md) · [简体中文](zh-CN.md) · **繁體中文** · [日本語](ja-JP.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

稀疏向量：一個從整數下標到任意值的映射，只儲存與空位預設值不同的位置

## 概念

下標可以為負，也不必連續

所以一個只有三個條目的向量就只佔三個條目 —— 無論這三個下標是 `0, 1, 2` 還是 `-10^9, 0, 10^9`

本函式庫刻意**不是**數學意義上的向量。它不帶任何算術；它就是一種資料結構，僅此而已

## 實作

同一個型別以三種語言提供。它們共用以下每一條規則 —— 差別只在寫法

| 語言 | 套件 | 目錄 | 狀態 |
| --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | [`typescript/`](../typescript/) | 已發布 |
| C++ | — | [`c++/`](../c++/) | 計畫中 |
| Rust | `sparse-vector-rs` | [`rust/`](../rust/) | 計畫中 |

每個實作目錄都各自帶有該語言的安裝、用法與 API 參考的說明文件。本頁定義它們共通的語意，因此不必重複寫三次

## 各實作共通的語意

### 空位預設值

向量建立時帶有一個預設值：所有沒有顯式條目的位置所回報的值。未指定時預設為 number 型別的 `0`

預設值之後可以替換。替換時會立即排除所有等於新預設值的條目

因為每個位置都有定義好的值，所以讀取是全定義的。任何整數 —— 已儲存或未儲存、在範圍內或遠在範圍外 —— 都會回傳一個值，而不是拋錯

### 等於空位預設值的條目永遠不會被保留

把預設值寫進某個位置，就等同於移除原本在那裡的東西。這正是讓結構保持稀疏的原因：記憶體是 O(k)，k 為真正不同於預設值的條目數，無論下標相隔多遠、負到什麼程度

### 排除採用嚴格相等判斷

每種語言各用自己最嚴格的相等判斷來排除 —— TypeScript 的 `===`、C++ 的 `==`、Rust 的 `PartialEq`。由此有兩個推論：

- `0`、`'0'`、`false`、`null` 是四個不同的值；只有完全相符才會排除該條目
- `NaN` 不等於它自己，所以即使預設值本身就是 `NaN`，存進去的 `NaN` 依然會被保留

### 序列化

條目是一個恰好含兩個鍵的普通物件：

| 鍵 | 型別 | 意義 |
| --- | --- | --- |
| `index` | 整數 | 位置，可為負 |
| `value` | 任意 | 存放在該處的值 |

向量序列化後就只有它的條目，並按下標升冪排列。預設值不屬於這個結構，因此往返時要把它一併帶上

## TypeScript

### 安裝

```sh
npm install @calbona/sparse-vector
```

### 用法

```ts
import { SV_vector } from '@calbona/sparse-vector';

const vector = new SV_vector(); // 空位預設值：number 型別的 0

vector.set(1_000_000, '很遠');
vector.set(-42, '負數');

vector.get(1_000_000); // '很遠'
vector.get(-42);       // '負數'
vector.get(7);         // 0
vector.size;           // 2
```

空位預設值可以指定，也可以之後修改；未儲存的位置一律按它取值

```ts
const counts = new SV_vector<number | null>(null); // 空位是 null
counts.set(3, 1);
counts.get(4); // null

counts.defaultValue = 0; // 空位改成 0
```

值的型別不受限制，什麼都可以

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

### API

#### `new SV_vector<T>(defaultValue?)`

建立向量

`defaultValue` 是空位預設值，即所有未儲存位置回報的值，預設為 number 型別的 `0`

`T` 預設為 `number`

#### 屬性

| 成員 | 說明 |
| --- | --- |
| `defaultValue: T` | 可讀可寫，賦值時會立即排除所有嚴格等於新預設值的條目 |
| `size: number` | 顯式儲存的條目數 |

#### 方法

| 方法 | 說明 |
| --- | --- |
| `get(index): T` | `index` 處的值；該處沒有顯式條目時回傳 `defaultValue` |
| `set(index, value): this` | 插入或修改，可鏈式呼叫 |
| `has(index): boolean` | `index` 處是否存在顯式條目 |
| `delete(index): boolean` | 刪除該處的顯式條目，回到預設值 |
| `clear(): void` | 刪除所有顯式條目，回到預設值 |
| `elements(): SV_element<T>[]` | 顯式條目，按下標升冪 |
| `keys(): number[]` | 已儲存的下標，升冪 |
| `values(): T[]` | 已儲存的值，按下標升冪 |
| `clone(): SV_vector<T>` | 獨立的複本 |
| `[Symbol.iterator]()` | 迭代顯式條目，按下標升冪 |
| `toJSON(): SV_element<T>[]` | 等同 `elements()`，因此 `JSON.stringify` 可直接使用 |

`index` 必須是整數 —— 可正可負；非整數會拋出 `TypeError`

取用資料範圍之外的位置不會拋錯，而是回傳預設值，這正是這個型別的意義所在

#### `SV_vector.from(elements, defaultValue?)`

由 `SV_element` 的可迭代物件建構，嚴格等於預設值的條目會被丟棄；同一下標重複出現時以最後一個為準

#### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

即上文所述的序列化形狀。往返時這樣寫：

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

### 開發

```sh
npm run build      # 編譯到 dist/
npm run typecheck  # 檢查 src 與測試的型別
npm test           # 先編譯，再測試
```

需要 Node 24+

本函式庫本身沒有執行時依賴

## C++

尚未發佈；計畫提供的型別為 `SV_vector` 與 `SV_element`。

## Rust

尚未發佈；計畫在 `sparse-vector-rs` crate 中提供 `SparseVector` 與 `Element`。

## 授權條款

MIT
