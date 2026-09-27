[English](../README.md) · [Deutsch](de-DE.md) · **Español** · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## La idea

- **Un vector disperso no es un vector**

	Su «índice» no empieza en cero ni recorre de izquierda a derecha: se parece más a como se escribe un número a mano, donde el índice hace de peso posicional, de modo que abarca desde el infinito positivo hasta el negativo. Por supuesto, ningún lenguaje tiene infinito de verdad: en TypeScript es `number`, en C++ `int64_t` y en Rust `i64`.

	Y como no es un vector matemático, no incorpora aritmética alguna. En ese sentido también es un diccionario, y además cada posición admite datos de cualquier tipo.

- **Cómo está hecho un vector disperso**

	Solo se almacenan las posiciones distintas del valor por defecto.

	Así, con varios objetos llamados «entradas» —un índice junto con un valor distinto del valor por defecto— formamos una lista, le añadimos el valor por defecto y ya está: el vector disperso queda listo. Sea cual sea el índice, si solo hay k entradas, la memoria es O(k).

## Bibliotecas

| Lenguaje | Paquete | Versión | Estado | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | publicado | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | publicado | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | publicado | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*Todavía sin publicar en vcpkg ni Conan*

Basta con apuntar CMake al repositorio

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

Un checkout junto a tu proyecto funciona igual, con `add_subdirectory(path/to/sparse-vector/c++)`
Un prefijo de instalación también exporta un paquete CMake, así que `find_package(sparse-vector)` funciona igualmente

### Rust

```sh
cargo add sparse-vector-rs
```

## Detalles de la semántica

### El valor por defecto de las posiciones vacías

- Al crear el vector se indica un valor por defecto

- Ese valor por defecto se puede sustituir después

- Toda posición tiene un valor definido, así que leer siempre tiene respuesta: cualquier entero devuelve un valor

### Una entrada igual al valor por defecto nunca se conserva

- Escribir el valor por defecto en una posición equivale a borrar lo que hubiera ahí

- Al sustituir el valor por defecto se descartan de inmediato las entradas iguales a él

- Este principio es lo que mantiene dispersa la estructura

### La igualdad

- Que una entrada sea igual al valor por defecto se decide según la práctica habitual de cada lenguaje: `===` en TypeScript, `operator==` en C++, `PartialEq` en Rust

- Los casos incómodos:
	- `-0.0` es igual a `0.0`
	- `NaN` no es igual a sí mismo
	- `0`, `'0'`, `false` y `null` son cuatro valores de cuatro tipos
	- `===` compara referencias de objetos, mientras que `operator==` y `PartialEq` comparan la estructura: dos objetos distintos con el mismo contenido son un valor en TypeScript y dos en C++ y Rust, así que el primero los conserva y los otros dos los descartan

- Cuando haga falta identidad, incorpórala a la igualdad del propio tipo; un tipo puntero te la da directamente: el `operator==` de `std::shared_ptr` compara punteros, y un `Rc<T>` puede envolverse en un newtype que compare con `Rc::ptr_eq`. Los README de C++ y de Rust lo detallan

## API

| Para qué | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| Construcción de un vector disperso (valor por defecto omitido) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (solo `f64`) / `SparseVector::default()` |
| Construcción de un vector disperso | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| Obtener el valor por defecto | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| Cambiar el valor por defecto | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| Obtener el número de entradas | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| Obtener la dimensión significativa | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| Obtener la dimensión positiva | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| Obtener la dimensión negativa | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| Obtener el valor en un índice | `get(index)` | `get(index)` | `get(index)` |
| Escribir el valor en un índice | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| Restablecer el valor en un índice | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| Restablecer todo el vector | `resetVector()` | `reset_vector()` | `reset_vector()` |
| Obtener todas las entradas, índice descendente | `elements()` | `elements()` | `elements()` |
| Obtener todas las entradas, índice ascendente | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| Obtener todos los índices no vacíos, descendente | `indexes()` | `indexes()` | `indexes()` |
| Obtener todos los índices no vacíos, ascendente | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| Obtener todos los valores no vacíos, descendente | `values()` | `values()` | `values()` |
| Obtener todos los valores no vacíos, ascendente | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| Obtener la (n+1)-ésima entrada desde la izquierda | `element(n)` | `element(n)` | `element(n)` |
| El índice de esa entrada | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| El valor de esa entrada | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| Obtener la (n+1)-ésima entrada desde la derecha | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| El índice de esa entrada | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| El valor de esa entrada | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| Obtener el (n+1)-ésimo dígito significativo por la izquierda | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| Obtener el (n+1)-ésimo dígito significativo por la derecha | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| Iteración | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| Copia | `clone()` | construcción por copia | `clone()` |
| Construcción en bloque | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*En Rust, obtener el valor y enumerar exige `T: Clone`, y escribir, restablecer o cambiar el valor por defecto exige `T: PartialEq`; en C++, de forma análoga, hace falta que el tipo sea copiable y disponga de `operator==`*

*Fuera de rango, o al medir un dígito significativo en un vector vacío, TypeScript lanza `TypeError` / `RangeError`, C++ lanza `std::out_of_range` y Rust entra en pánico*

## Licencia

MIT
