[English](../README.md) · [简体中文](zh-CN.md) · **繁體中文** · [日本語](ja-JP.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

TypeScript 的稀疏向量：一個從整數下標到任意值的映射，只儲存與空位預設值不同的位置

下標可以為負，也不必連續

所以一個只有三個條目的向量就只佔三個條目 —— 無論這三個下標是 `0, 1, 2` 還是 `-10^9, 0, 10^9`

本函式庫刻意**不是**數學意義上的向量

## 安裝

```sh
npm install sparse-vector
```

## 用法

```ts
import { SV_vector } from 'sparse-vector';

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

## API

### `new SV_vector<T>(defaultValue?)`

建立向量

`defaultValue` 是空位預設值，即所有未儲存位置回報的值，預設為 number 型別的 `0`

`T` 預設為 `number`

### 屬性

| 成員 | 說明 |
| --- | --- |
| `defaultValue: T` | 可讀可寫，賦值時會立即排除所有嚴格等於新預設值的條目 |
| `size: number` | 顯式儲存的條目數 |

### 方法

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
| `clone(): SV_vector<T>` | 複製 |
| `[Symbol.iterator]()` | 迭代顯式條目，按下標升冪 |
| `toJSON(): SV_element<T>[]` | 等同 `elements()`，因此 `JSON.stringify` 可直接使用 |

`index` 必須是整數 —— 可正可負；非整數會拋出 `TypeError`

取用資料範圍之外的位置不會拋錯，而是回傳預設值，這正是這個型別的意義所在

### `SV_vector.from(elements, defaultValue?)`

由 `SV_element` 的可迭代物件建構，嚴格等於預設值的條目會被丟棄；同一下標重複出現時以最後一個為準

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

這是序列化格式：一個恰好含 `index` 與 `value` 兩個鍵的普通 JSON 物件

注意 `toJSON()` 只輸出條目本身，預設值不在這套結構裡，因此 JSON 往返時要把預設值一併帶上

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

## 排除規則

等於空位預設值的條目永遠不會被保留

排除採用**嚴格相等**判斷，由此有兩個推論：

- `0`、`'0'`、`false`、`null` 是四個不同的值，只有 `===` 全等才會被排除
- `NaN` 與 `NaN` 不嚴格相等，所以即使預設值本身就是 `NaN`，存進去的 `NaN` 依然會被保留

## 開發

```sh
npm run build      # 編譯到 dist/
npm run typecheck  # 檢查 src 與測試的型別
npm test           # 先編譯，再測試
```

需要 Node 24+

本函式庫本身沒有執行時依賴

## License

MIT
