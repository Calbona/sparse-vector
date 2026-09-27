[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · **한국어** · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## 개요

- **희소 벡터는 벡터가 아니다**

	그 「인덱스」는 0에서 시작하지도 않고 왼쪽에서 오른쪽으로 흐르지도 않는다. 오히려 사람이 수를 손으로 쓰는 방식에 가깝고, 인덱스가 자릿값의 무게를 대신하므로 양의 무한대에서 음의 무한대까지 이어진다. 물론 언어에 무한대가 있는 것은 아니다. TypeScript에서는 `number`, C++에서는 `int64_t`, Rust에서는 `i64`이다.

	그리고 수학적인 벡터가 아니므로 연산도 지니지 않는다. 그런 점에서 이것은 사전이기도 하며, 모든 자리에는 실제로 어떤 타입의 데이터든 담을 수 있다.

- **희소 벡터가 만들어지는 원리**

	기본값과 다른 위치만 저장한다.

	그래서 「요소」라 부르는, 인덱스 하나와 기본값과는 다른 값 하나를 짝지은 객체를 여러 개 늘어놓아 목록으로 만들고, 거기에 기본값을 더하면, 짜잔, 희소 벡터가 완성된다. 인덱스가 어디에 있든 요소가 k개뿐이라면 메모리는 O(k)이다.

## 라이브러리

| 언어 | 패키지 | 버전 | 상태 | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | 배포됨 | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | 배포됨 | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | 배포됨 | [`rust/`](../rust/) |

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

### 빈 자리의 기본값

- 희소 벡터를 만들 때 기본값을 지정한다

- 그 기본값은 나중에 교체할 수 있다

- 모든 자리에 값이 정해져 있으므로 읽기는 늘 답을 준다. 어떤 정수를 넣어도 값이 나온다

### 기본값과 같은 요소는 결코 보관되지 않는다

- 어떤 자리에 기본값을 쓰는 것은 그 자리에 있던 것을 지우는 것과 같다

- 기본값을 교체하면 그와 같은 요소는 곧바로 제거된다

- 바로 이 원리가 구조를 희소하게 유지한다

### 동등성 판정

- 어떤 요소가 기본값과 같은지는 각 언어의 일상적인 판정을 따른다. TypeScript는 `===`, C++는 `operator==`, Rust는 `PartialEq`

- 까다로운 경우:
	- `-0.0`은 `0.0`과 같다
	- `NaN`은 자기 자신과 같지 않다
	- `0`, `'0'`, `false`, `null`은 서로 다른 네 타입의 값이다
	- `===`는 객체 참조를 비교하고, `operator==`와 `PartialEq`는 구조를 비교한다. 내용이 같지만 서로 독립된 두 객체는 TypeScript에서 하나의 값이고 C++와 Rust에서는 두 개의 값이므로, 앞의 것은 남기고 나머지 둘은 버린다

- 동일성이 필요하면 그것을 타입 자체의 동등성에 넣으면 된다. 포인터 타입을 쓰면 곧바로 얻을 수 있다. `std::shared_ptr`의 `operator==`는 포인터를 비교하고, `Rc<T>`는 `Rc::ptr_eq`로 비교하는 newtype으로 감싸면 된다

## API

| 용도 | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| 희소 벡터 생성(기본값 생략) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (`f64`만) / `SparseVector::default()` |
| 희소 벡터 생성 | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| 기본값 얻기 | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| 기본값 바꾸기 | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| 요소 수 얻기 | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| 유효 차원 얻기 | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| 양의 유효 차원 얻기 | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| 음의 유효 차원 얻기 | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| 인덱스의 값 얻기 | `get(index)` | `get(index)` | `get(index)` |
| 인덱스에 값 쓰기 | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| 인덱스의 값 초기화 | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| 벡터 전체 초기화 | `resetVector()` | `reset_vector()` | `reset_vector()` |
| 모든 요소, 인덱스 내림차순 | `elements()` | `elements()` | `elements()` |
| 모든 요소, 인덱스 오름차순 | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| 비어 있지 않은 인덱스 전부, 내림차순 | `indexes()` | `indexes()` | `indexes()` |
| 비어 있지 않은 인덱스 전부, 오름차순 | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| 비어 있지 않은 값 전부, 내림차순 | `values()` | `values()` | `values()` |
| 비어 있지 않은 값 전부, 오름차순 | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| 왼쪽에서 (n+1)번째 요소 | `element(n)` | `element(n)` | `element(n)` |
| 그 요소의 인덱스 | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| 그 요소의 값 | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| 오른쪽에서 (n+1)번째 요소 | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| 그 요소의 인덱스 | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| 그 요소의 값 | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| 왼쪽에서 (n+1)번째 유효 숫자 | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| 오른쪽에서 (n+1)번째 유효 숫자 | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| 순회 | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| 복사 | `clone()` | 복사 생성 | `clone()` |
| 일괄 생성 | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*Rust에서는 값 읽기와 열거에 `T: Clone`, 쓰기·초기화·기본값 변경에 `T: PartialEq`가 필요하다. C++에서는 복사 가능성과 `operator==`가 필요하다*

*범위를 벗어나거나 빈 벡터에서 유효 숫자를 구하려 하면 TypeScript는 `TypeError` / `RangeError`를 던지고, C++는 `std::out_of_range`를 던지며, Rust는 panic한다*

## 라이선스

MIT
