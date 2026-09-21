[English](../README.md) · **简体中文** · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

TypeScript 的稀疏向量：一个从整数下标到任意值的映射，只存储与空位默认值不同的位置

下标可以为负，也不必连续

所以一个只有三个条目的向量就只占三个条目 —— 无论这三个下标是 `0, 1, 2` 还是 `-10^9, 0, 10^9`

本库刻意**不是**数学意义上的向量

## 安装

```sh
npm install sparse-vector
```

## 用法

```ts
import { SV_vector } from 'sparse-vector';

const vector = new SV_vector(); // 空位默认值：number 类型的 0

vector.set(1_000_000, '很远');
vector.set(-42, '负数');

vector.get(1_000_000); // '很远'
vector.get(-42);       // '负数'
vector.get(7);         // 0
vector.size;           // 2
```

空位默认值可以指定，也可以之后修改；未存储的位置一律按它取值

```ts
const counts = new SV_vector<number | null>(null); // 空位是 null
counts.set(3, 1);
counts.get(4); // null

counts.defaultValue = 0; // 空位改成 0
```

值的类型不受限制，什么都可以

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

## API

### `new SV_vector<T>(defaultValue?)`

创建向量

`defaultValue` 是空位默认值，即所有未存储位置报告的值，缺省为 number 类型的 `0`

`T` 缺省为 `number`

### 属性

| 成员 | 说明 |
| --- | --- |
| `defaultValue: T` | 可读可写，赋值时会立即剔除所有严格等于新默认值的条目 |
| `size: number` | 显式存储的条目数 |

### 方法

| 方法 | 说明 |
| --- | --- |
| `get(index): T` | `index` 处的值；该处没有显式条目时返回 `defaultValue` |
| `set(index, value): this` | 插入或修改，可链式调用 |
| `has(index): boolean` | `index` 处是否存在显式条目 |
| `delete(index): boolean` | 删除该处的显式条目，回到默认值 |
| `clear(): void` | 删除所有显式条目，回到默认值 |
| `elements(): SV_element<T>[]` | 显式条目，按下标升序 |
| `keys(): number[]` | 已存储的下标，升序 |
| `values(): T[]` | 已存储的值，按下标升序 |
| `clone(): SV_vector<T>` | 克隆 |
| `[Symbol.iterator]()` | 迭代显式条目，按下标升序 |
| `toJSON(): SV_element<T>[]` | 等同 `elements()`，因此 `JSON.stringify` 可直接使用 |

`index` 必须是整数 —— 可正可负；非整数会抛出 `TypeError`

取用数据范围之外的位置不会抛错，而是返回默认值，这正是这个类型的意义所在

### `SV_vector.from(elements, defaultValue?)`

由 `SV_element` 的可迭代对象构造，严格等于默认值的条目会被丢弃；同一下标重复出现时以最后一个为准

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

这是序列化格式：一个恰好含 `index` 与 `value` 两个键的普通 JSON 对象

注意 `toJSON()` 只输出条目本身，默认值不在这套结构里，因此 JSON 往返时要把默认值一并带上

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

## 剔除规则

等于空位默认值的条目永远不被保留

剔除采用**严格相等**判断，由此有两个推论：

- `0`、`'0'`、`false`、`null` 是四个不同的值，只有 `===` 全等才会被剔除
- `NaN` 与 `NaN` 不严格相等，所以即使默认值本身就是 `NaN`，存进去的 `NaN` 依然会被保留

## 开发

```sh
npm run build      # 编译到 dist/
npm run typecheck  # 检查 src 与测试的类型
npm test           # 先编译，再测试
```

需要 Node 24+

本库本身没有运行时依赖

## License

MIT
