[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · **Русский** · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Идея

- **Что такое разреженный вектор**

	Разреженный вектор — не математический вектор. Он устроен скорее как число, записанное от руки: его «индекс» тянется от плюс бесконечности до минус бесконечности. Разумеется, в языках программирования бесконечности нет, поэтому в TypeScript это `number`, в C++ — `int64_t`, в Rust — `i64`.

	А раз это не вектор, то и алгебраических операций над ним не бывает.

- **Как устроен разреженный вектор**

	Спросите про значение в любом разряде — и получите ответ. Как так выходит?

	Мы храним только те позиции, что отличаются от значения по умолчанию, а само значение по умолчанию — отдельно. Спросите про позицию, где ничего не сохранено, — и вектор вернёт вам значение по умолчанию.

## Библиотеки

| Язык | Пакет | Версия | Статус | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | выпущено | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | выпущено | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | выпущено | [`rust/`](../rust/) |

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

### Вектор

- Это не математический вектор и не массив в компьютере, а та особая структура данных, которую даёт библиотека.

### Разреженность

- Вместимость такого вектора куда больше числа его элементов: часть разрядов не хранит данные явно.

### Индекс

- Индексом вектора служит любое целое число, положительное или отрицательное; по смыслу это что-то вроде единиц или десятков.

### Значение

- То, что мы действительно хотим хранить, — аналог цифры в разряде сотен или тысяч, только тип не обязательно числовой. Это может быть что угодно.

### Элемент

- Один индекс и одно значение. Получившийся объект и есть элемент; именно элементы вектор и хранит на самом деле.

### Значение по умолчанию

- Там, куда данные явно не записаны, находится значение по умолчанию. Это как с записью числа: нули мы обычно не пишем — пишем 1, а не 0001.000, верно?

- После того как вектор построен, значение по умолчанию можно заменить. Это удивительно: не знаю, для чего это может пригодиться, но я оставил вам эту возможность.

### Автоматическое поддержание минимальной памяти

- Когда значение по умолчанию заменяют, все элементы, равные ему, тут же удаляются.

- То же самое при замене предиката равенства.

- Если записать в разряд значение по умолчанию, это равносильно удалению того, что там было.

### Равенство

- Считать ли значение равным значению по умолчанию? По обычному в каждом языке способу сравнения.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- Вектору можно задать и собственный предикат: он принимает два аргумента — сравниваемое значение и текущее значение по умолчанию — и возвращает логическое значение, заменяя собой обычное сравнение.

- Сравнение двух векторов — это две отдельные именованные операции, и с удалением элементов они не связаны: `isEqualTo` / `is_equal_to` определяет, равны ли два вектора, а `differences` перечисляет элементы, которыми получатель отличается от другого. Обе операции **опираются на предикат получателя**, поэтому при разных предикатах `a.isEqualTo(b)` и `b.isEqualTo(a)` могут дать разные ответы.

- Учтите, что у обычных способов сравнения в языках есть неочевидные случаи.
	- `NaN` не равен самому себе.
	- `0`, `'0'`, `false`, `null` — типы у всех разные, поэтому они не равны.
	- `===` сравнивает ссылки на объекты, а `operator==` и `PartialEq` сравнивают структуру.

## API

| Назначение | TypeScript | C++ | Rust | Возвращаемый тип |
| --- | --- | --- | --- | --- |
| Создание разреженного вектора (значение по умолчанию опущено) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | новый вектор |
| Создание разреженного вектора | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | новый вектор |
| Получить значение по умолчанию | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts значение, cpp и rust ссылка |
| Изменить значение по умолчанию | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts ничего, cpp и rust логическое значение |
| Получить предикат равенства | `getEquality` | `get_equality()` | `get_equality()` | ts предикат или `undefined`, cpp и rust предикат или пусто |
| Изменить предикат равенства | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts ничего, cpp и rust логическое значение |
| Получить число элементов | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | целое число |
| Получить значащую размерность | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | целое число |
| Получить положительную размерность | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | целое число |
| Получить отрицательную размерность | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | целое число |
| Получить значение по индексу | `get(index)` | `get(index)` | `get(index)` | тип значения |
| Записать значение по индексу | `set(index, value)` | `set(index, value)` | `set(index, value)` | сам вектор |
| Сбросить значение по индексу | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | логическое значение |
| Сбросить весь вектор, не трогая значение по умолчанию | `resetVector()` | `reset_vector()` | `reset_vector()` | логическое значение |
| Получить все элементы, индекс по убыванию | `elements()` | `elements()` | `elements()` | массив элементов |
| Получить все элементы, индекс по возрастанию | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | массив элементов |
| Получить все индексы, по убыванию | `indexes()` | `indexes()` | `indexes()` | массив индексов |
| Получить все индексы, по возрастанию | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | массив индексов |
| Получить все значения, по убыванию | `values()` | `values()` | `values()` | массив значений |
| Получить все значения, по возрастанию | `invertedValues()` | `inverted_values()` | `inverted_values()` | массив значений |
| Получить (n+1)-й элемент слева | `element(n)` | `element(n)` | `element(n)` | элемент |
| Индекс этого элемента | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | индекс |
| Значение этого элемента | `elementValue(n)` | `element_value(n)` | `element_value(n)` | значение |
| Получить (n+1)-й элемент справа | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | элемент |
| Индекс этого элемента | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | индекс |
| Значение этого элемента | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | значение |
| Получить (n+1)-ю значащую цифру слева | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | значение |
| Получить (n+1)-ю значащую цифру справа | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | значение |
| Обход | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | итератор (выдаёт элементы в порядке убывания индекса) |
| Копирование | `clone()` | копирующий конструктор | `clone()` | новый вектор |
| Массовое создание | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | новый вектор |
| Проверить, равны ли два вектора | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | логическое значение |
| Перечислить элементы, которыми вектор отличается от другого | `differences(other)` | `differences(other)` | `differences(other)` | массив элементов |

*В Rust для получения значения и перечисления требуется `T: Clone`, для записи, сброса и смены значения по умолчанию — `T: PartialEq`, для проверки векторов на равенство — тоже `T: PartialEq`, а для вычисления различий — ещё и `T: Clone`; в C++ нужны копируемость и `operator==`*

*Вычисление различий требует, чтобы оба значения по умолчанию были одного типа и считались равными по предикату получателя; иначе TypeScript бросает `TypeError`, C++ — `std::invalid_argument`, а Rust паникует*

*При выходе за диапазон или при измерении значащей цифры на пустом векторе TypeScript бросает `TypeError` / `RangeError`, C++ бросает `std::out_of_range`, а Rust паникует*

## Лицензия

MIT
