[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · **한국어** · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## 개요

- **희소 벡터의 정의**

	희소 벡터는 수학에서 말하는 벡터가 아니다. 오히려 손으로 수를 적는 방식에 가깝다. 인덱스는 양의 무한대에서 음의 무한대까지 이어진다. 물론 컴퓨터 언어에 무한대가 있는 것은 아니므로, TypeScript에서는 `number`, C++에서는 `int64_t`, Rust에서는 `i64`이다.

	벡터가 아니니 대수 연산을 할 수 없는 것은 당연하다.

- **희소 벡터의 원리**

	어느 자릿값의 값을 물어도 답이 나온다. 이것이 어떻게 가능한가?

	기본값과 다른 위치만 저장하고, 기본값 자체를 따로 하나 저장한다. 저장하지 않은 위치를 조회하면 벡터가 기본값을 내준다.

## 라이브러리

| 언어 | 패키지 | 버전 | 상태 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | 배포됨 | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | 배포됨 | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | 배포됨 | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*아직 vcpkg나 Conan에는 없다*

CMake가 저장소를 가리키게 하면 된다

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

프로젝트 옆에 체크아웃을 두어도 마찬가지이며, `add_subdirectory(path/to/sparse-vector/c++)`라고 쓴다
설치 경로도 CMake 패키지를 내보내므로 `find_package(sparse-vector)`도 쓸 수 있다

### Rust

```sh
cargo add sparse-vector-rs
```

## 의미론

### 벡터

- 수학의 벡터도, 컴퓨터의 배열도 아닌, 이 라이브러리가 제공하는 특별한 데이터 구조를 가리킨다.

### 희소

- 이 벡터는 용량이 요소 개수보다 훨씬 크고, 어떤 자릿값에는 데이터가 명시적으로 저장되지 않는다는 뜻이다.

### 인덱스

- 벡터의 인덱스는 정수 전체이며 양수도 음수도 될 수 있다. 일의 자리, 십의 자리와 같은 개념을 나타낸다.

### 값

- 실제로 저장하려는 대상이며, 백의 자리 숫자나 천의 자리 숫자에 비유할 수 있다. 다만 타입이 반드시 수일 필요는 없고 무엇이든 될 수 있다.

### 요소

- 인덱스 하나와 값 하나로 이루어진 객체를 요소라고 하며, 이것이 벡터가 실제로 저장하는 것이다.

### 기본값

- 희소 벡터에서 데이터가 명시적으로 저장되지 않은 자리가 바로 기본값이다. 수를 쓸 때 쓰지 않고 남겨 두는 0에 비유할 수 있다. 보통 0001.000이라고 쓰지 않고 1이라고 쓴다.

- 벡터를 만든 뒤에는 기본값을 바꿀 수 있다. 신기한 일이지만 어디에 쓸지는 알 수 없다. 그래도 이런 능력은 미리 마련해 두었다.

### 최소 메모리 자동 유지

- 기본값을 바꾸면 값이 기본값과 같은 요소가 곧바로 지워진다.

- 동등성 판정을 바꿀 때도 마찬가지다.

- 어떤 자릿값에 쓴 값이 기본값이라면, 그 자리에 있던 것을 지우는 것과 같다.

### 동등성 판정

- 값이 기본값과 같은가? 각 언어의 일반적인 판정 방법을 따른다.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- 벡터에 판정을 직접 지정할 수도 있다. 이 판정은 비교할 값과 현재 기본값, 두 매개변수를 받아 불리언을 반환하며, 일반적인 판정을 대신한다.

- 두 벡터를 비교하는 일은 이름이 붙은 두 가지 작업이며, 제거와는 별개다. `isEqualTo` / `is_equal_to`는 두 벡터가 같은지 판정하고, `differences`는 수신자가 상대와 다른 요소를 나열한다. 둘 다 **수신자의 판정을 기준으로 삼으므로**, 양쪽 판정이 다르면 `a.isEqualTo(b)`와 `b.isEqualTo(a)`가 서로 다른 답을 낼 수 있다.

- 각 언어의 동등성 판정 방법에는 직관을 벗어나는 경우가 있다.
	- `NaN`은 자기 자신과 같지 않다.
	- `0`, `'0'`, `false`, `null`은 타입이 모두 다르므로 서로 같지 않다.
	- `===`는 객체의 참조를 비교하지만, `operator==`와 `PartialEq`는 구조를 비교한다.

## API

| 용도 | TypeScript | C++ | Rust | 반환 타입 |
| --- | --- | --- | --- | --- |
| 희소 벡터 생성(기본값 생략) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | 새 벡터 |
| 희소 벡터 생성 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | 새 벡터 |
| 기본값 얻기 | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts 값, cpp·rust 참조 |
| 기본값 바꾸기 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts 없음, cpp·rust 불리언 |
| 동등성 판정 얻기 | `getEquality` | `get_equality()` | `get_equality()` | ts 판정 또는 `undefined`, cpp·rust 판정 또는 없음 |
| 동등성 판정 바꾸기 | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts 없음, cpp·rust 불리언 |
| 요소 수 얻기 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | 정수 |
| 유효 차원 얻기 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | 정수 |
| 양의 유효 차원 얻기 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | 정수 |
| 음의 유효 차원 얻기 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | 정수 |
| 인덱스의 값 얻기 | `get(index)` | `get(index)` | `get(index)` | 값의 타입 |
| 인덱스에 값 쓰기 | `set(index, value)` | `set(index, value)` | `set(index, value)` | 벡터 자신 |
| 인덱스의 값 초기화 | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | 불리언 |
| 벡터 전체 초기화 | `resetVector()` | `reset_vector()` | `reset_vector()` | 불리언 |
| 모든 요소, 인덱스 내림차순 | `elements()` | `elements()` | `elements()` | 요소 배열 |
| 모든 요소, 인덱스 오름차순 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | 요소 배열 |
| 비어 있지 않은 인덱스 전부, 내림차순 | `indexes()` | `indexes()` | `indexes()` | 인덱스 배열 |
| 비어 있지 않은 인덱스 전부, 오름차순 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | 인덱스 배열 |
| 비어 있지 않은 값 전부, 내림차순 | `values()` | `values()` | `values()` | 값 배열 |
| 비어 있지 않은 값 전부, 오름차순 | `invertedValues()` | `inverted_values()` | `inverted_values()` | 값 배열 |
| 왼쪽에서 (n+1)번째 요소 | `element(n)` | `element(n)` | `element(n)` | 요소 |
| 그 요소의 인덱스 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | 인덱스 |
| 그 요소의 값 | `elementValue(n)` | `element_value(n)` | `element_value(n)` | 값 |
| 오른쪽에서 (n+1)번째 요소 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | 요소 |
| 그 요소의 인덱스 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | 인덱스 |
| 그 요소의 값 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | 값 |
| 왼쪽에서 (n+1)번째 유효 숫자 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | 값 |
| 오른쪽에서 (n+1)번째 유효 숫자 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | 값 |
| 순회 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | 반복자(인덱스 내림차순으로 요소를 빌려준다) |
| 복사 | `clone()` | 복사 생성 | `clone()` | 새 벡터 |
| 일괄 생성 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | 새 벡터 |
| 두 벡터의 동등성 판정 | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | 불리언 |
| 다른 벡터와 차이가 나는 요소 나열 | `differences(other)` | `differences(other)` | `differences(other)` | 요소 배열 |

*Rust에서는 값 읽기와 열거에 `T: Clone`, 쓰기·초기화·기본값 변경에 `T: PartialEq`가 필요하다. 두 벡터의 동등성 판정에도 `T: PartialEq`가 필요하고, 차이를 구하려면 여기에 더해 `T: Clone`도 필요하다. C++에서는 복사 가능성과 `operator==`가 필요하다*

*차이를 구하려면 두 기본값의 타입이 같고 수신자의 판정으로 서로 같아야 한다. 그렇지 않으면 TypeScript는 `TypeError`를 던지고, C++는 `std::invalid_argument`를 던지며, Rust는 panic한다*

*범위를 벗어나거나 빈 벡터에서 유효 숫자를 구하려 하면 TypeScript는 `TypeError` / `RangeError`를 던지고, C++는 `std::out_of_range`를 던지며, Rust는 panic한다*

## 라이선스

MIT
