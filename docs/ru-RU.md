[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · **Русский** · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Идея

- **Разреженный вектор — это не вектор**

	Его «индекс» начинается не с нуля и идёт не слева направо: он скорее похож на то, как число пишут от руки, где индекс играет роль веса разряда, так что он простирается от минус бесконечности до плюс бесконечности. Разумеется, из-за ограничений языка бесконечности на самом деле нет: в TypeScript это `number`, в C++ — `int64_t`, в Rust — `i64`.

	А раз это не математический вектор, то и арифметики в нём нет. В этом смысле он ещё и словарь, к тому же в каждой позиции действительно можно хранить данные любого типа.

- **Как устроен разреженный вектор**

	Хранятся только те позиции, что отличны от значения по умолчанию.

	Итак, берём несколько объектов, называемых «записями» — индекс плюс значение, отличное от значения по умолчанию, — выстраиваем из них список, добавляем значение по умолчанию — и готово, разреженный вектор собран. Какими бы ни были индексы, если записей всего k, память составляет O(k).

## Библиотеки

| Язык | Пакет | Версия | Статус | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | выпущено | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | выпущено | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | выпущено | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*Пока не опубликована в vcpkg или Conan*

Достаточно указать CMake на репозиторий

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

Рабочая копия рядом с вашим проектом работает так же, через `add_subdirectory(path/to/sparse-vector/c++)`
Префикс установки также экспортирует пакет CMake, поэтому `find_package(sparse-vector)` тоже работает

### Rust

```sh
cargo add sparse-vector-rs
```

## Подробности семантики

### Значение по умолчанию для пустых позиций

- При создании разреженного вектора задаётся значение по умолчанию

- Позже это значение по умолчанию можно заменить

- В каждой позиции значение определено, поэтому чтение всегда даёт результат: любое целое число вернёт значение

### Запись, равная значению по умолчанию, никогда не сохраняется

- Запись значения по умолчанию в позицию равнозначна стиранию того, что там было

- При замене значения по умолчанию немедленно отбрасываются записи, равные ему

- Именно этот принцип и сохраняет структуру разреженной

### Равенство

- То, равна ли запись значению по умолчанию, определяется обычной практикой каждого языка: `===` в TypeScript, `operator==` в C++, `PartialEq` в Rust

- Неудобные случаи:
	- `-0.0` равен `0.0`
	- `NaN` не равен сам себе
	- `0`, `'0'`, `false` и `null` — четыре значения четырёх разных типов
	- `===` сравнивает ссылки на объекты, а `operator==` и `PartialEq` сравнивают структуру: два различных объекта с одинаковым содержимым — это одно значение в TypeScript и два в C++ и Rust, поэтому TypeScript их сохраняет, а C++ и Rust отбрасывают

- Когда нужна именно тождественность, заложите её в равенство самого типа; тип-указатель даёт её сразу: `operator==` у `std::shared_ptr` сравнивает указатели, а `Rc<T>` можно обернуть в newtype, сравнивающий через `Rc::ptr_eq`. Подробности — в README для C++ и Rust

## API

| Назначение | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| Создание разреженного вектора (значение по умолчанию опущено) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (только `f64`) / `SparseVector::default()` |
| Создание разреженного вектора | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| Получить значение по умолчанию | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| Изменить значение по умолчанию | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| Получить число записей | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| Получить значащую размерность | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| Получить положительную размерность | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| Получить отрицательную размерность | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| Получить значение по индексу | `get(index)` | `get(index)` | `get(index)` |
| Записать значение по индексу | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| Сбросить значение по индексу | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| Сбросить весь вектор | `resetVector()` | `reset_vector()` | `reset_vector()` |
| Получить все записи, индекс по убыванию | `elements()` | `elements()` | `elements()` |
| Получить все записи, индекс по возрастанию | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| Получить все непустые индексы, по убыванию | `indexes()` | `indexes()` | `indexes()` |
| Получить все непустые индексы, по возрастанию | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| Получить все непустые значения, по убыванию | `values()` | `values()` | `values()` |
| Получить все непустые значения, по возрастанию | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| Получить (n+1)-ю запись слева | `element(n)` | `element(n)` | `element(n)` |
| Индекс этой записи | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| Значение этой записи | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| Получить (n+1)-ю запись справа | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| Индекс этой записи | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| Значение этой записи | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| Получить (n+1)-ю значащую цифру слева | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| Получить (n+1)-ю значащую цифру справа | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| Обход | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| Копирование | `clone()` | копирующий конструктор | `clone()` |
| Массовое создание | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*В Rust для получения значения и перечисления требуется `T: Clone`, а для записи, сброса и смены значения по умолчанию — `T: PartialEq`; в C++ соответственно нужны копируемость и `operator==`*

*При выходе за диапазон или при измерении значащей цифры на пустом векторе TypeScript бросает `TypeError` / `RangeError`, C++ бросает `std::out_of_range`, а Rust паникует*

## Лицензия

MIT
