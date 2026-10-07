[English](../README.md) · [Deutsch](de-DE.md) · **Español** · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## La idea

- **Qué es un vector disperso**

	Un vector disperso no es un vector matemático: se parece más bien a la forma en que escribes un número a mano, con el «índice» extendiéndose desde el infinito positivo hasta el negativo. Claro que ningún lenguaje tiene infinito de verdad, así que en TypeScript es `number`, en C++ `int64_t` y en Rust `i64`.

	Y, al no ser un vector, tampoco admite aritmética de ningún tipo.

- **Cómo funciona un vector disperso**

	Pregunta por el valor de cualquier peso posicional y siempre obtienes respuesta. ¿Cómo se consigue?

	Solo guardamos las posiciones cuyo valor difiere del valor por defecto y, además, el propio valor por defecto. Así, cuando consultas una posición donde no se guardó nada, el vector te devuelve el valor por defecto.

## Bibliotecas

| Lenguaje | Paquete | Versión | Estado | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | publicado | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | publicado | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | publicado | [`rust/`](../rust/) |

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

### El vector

- No se refiere a un vector matemático ni a un array de la informática, sino a esta estructura de datos especial que ofrece la biblioteca.

### Disperso

- Significa que la capacidad del vector supera con creces su número de elementos: algunos pesos posicionales nunca se almacenaron de forma explícita.

### El índice

- El índice del vector es el conjunto de todos los enteros, positivos o negativos; representa algo así como las unidades o las decenas.

### El valor

- Lo que de verdad queremos almacenar, el análogo del dígito de las centenas o de los millares; eso sí, el tipo no tiene por qué ser un número: puede ser cualquier cosa.

### El elemento

- Un índice más un valor componen un objeto llamado elemento, y los elementos son lo que el vector almacena de verdad.

### El valor por defecto

- Toda posición del vector disperso donde no se almacenó nada de forma explícita tiene el valor por defecto. Piénsalo como cuando escribes un número: te ahorras los ceros y escribes 1 en lugar de 0001.000, ¿verdad?

- Una vez construido el vector, el valor por defecto se puede sustituir. Es curioso, no sé muy bien para qué servirá, pero te he reservado esa capacidad.

### Memoria mínima mantenida automáticamente

- Al sustituir el valor por defecto, se descartan de inmediato los elementos iguales a él.

- Lo mismo al sustituir el predicado de igualdad.

- Escribir el valor por defecto en un peso posicional equivale a borrar lo que hubiera allí.

### La igualdad

- ¿Un valor es igual al valor por defecto? Según el método habitual de cada lenguaje.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- También puedes darle al vector un predicado: recibe el valor a comparar y el valor por defecto actual y devuelve un booleano; sustituye al método habitual.

- Comparar dos vectores comprende dos operaciones con nombre propio y no es lo mismo que el descarte: `isEqualTo` / `is_equal_to` decide si dos vectores son iguales, y `differences` enumera los elementos en que el receptor difiere del otro. Ambas **se rigen por el predicado del receptor**; si los dos llevan predicados distintos, `a.isEqualTo(b)` y `b.isEqualTo(a)` pueden dar respuestas diferentes.

- Ojo: los métodos de igualdad de cada lenguaje tienen algunos casos que pueden resultar contraintuitivos.
	- `NaN` no es igual a sí mismo.
	- `0`, `'0'`, `false` y `null` son de tipos distintos, así que no son iguales.
	- `===` compara la referencia de los objetos, mientras que `operator==` y `PartialEq` comparan la estructura.

## API

| Para qué | TypeScript | C++ | Rust | Tipo de retorno |
| --- | --- | --- | --- | --- |
| Construir un vector (valor por defecto omitido) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | Nuevo vector |
| Construir un vector | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | Nuevo vector |
| Acceder al valor por defecto | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts: valor; cpp, rust: referencia |
| Actualizar el valor por defecto | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts: ninguno; cpp, rust: booleano |
| Acceder al predicado de igualdad | `getEquality` | `get_equality()` | `get_equality()` | ts: predicado o `undefined`; cpp, rust: predicado o vacío |
| Actualizar el predicado de igualdad | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts: ninguno; cpp, rust: booleano |
| Obtener el número de elementos | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | Entero |
| Obtener la dimensión significativa | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | Entero |
| Obtener la dimensión positiva | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | Entero |
| Obtener la dimensión negativa | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | Entero |
| Obtener el valor en un índice | `get(index)` | `get(index)` | `get(index)` | El tipo del valor |
| Escribir el valor en un índice | `set(index, value)` | `set(index, value)` | `set(index, value)` | El propio vector |
| Restablecer el valor en un índice | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | Booleano |
| Restablecer todo el vector, sin tocar el valor por defecto | `resetVector()` | `reset_vector()` | `reset_vector()` | Booleano |
| Obtener todos los elementos, índice descendente | `elements()` | `elements()` | `elements()` | Array de elementos |
| Obtener todos los elementos, índice ascendente | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | Array de elementos |
| Obtener todos los índices, descendente | `indexes()` | `indexes()` | `indexes()` | Array de índices |
| Obtener todos los índices, ascendente | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | Array de índices |
| Obtener todos los valores, descendente | `values()` | `values()` | `values()` | Array de valores |
| Obtener todos los valores, ascendente | `invertedValues()` | `inverted_values()` | `inverted_values()` | Array de valores |
| Obtener el (n+1)-ésimo elemento desde la izquierda | `element(n)` | `element(n)` | `element(n)` | Elemento |
| El índice de ese elemento | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | Índice |
| El valor de ese elemento | `elementValue(n)` | `element_value(n)` | `element_value(n)` | Valor |
| Obtener el (n+1)-ésimo elemento desde la derecha | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | Elemento |
| El índice de ese elemento | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | Índice |
| El valor de ese elemento | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | Valor |
| Obtener el (n+1)-ésimo dígito significativo por la izquierda | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | Valor |
| Obtener el (n+1)-ésimo dígito significativo por la derecha | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | Valor |
| Iteración | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | Iterador (presta los elementos en índice descendente) |
| Copia | `clone()` | construcción por copia | `clone()` | Nuevo vector |
| Construcción en bloque | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | Nuevo vector |
| Comprobar si dos vectores son iguales | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | Booleano |
| Listar los elementos que difieren de otro vector | `differences(other)` | `differences(other)` | `differences(other)` | Array de elementos |

*En Rust, obtener el valor y enumerar exige `T: Clone`, y escribir, restablecer o cambiar el valor por defecto exige `T: PartialEq`; comprobar si dos vectores son iguales exige también `T: PartialEq`, y calcular la diferencia exige además `T: Clone`; en C++, de forma análoga, hace falta que el tipo sea copiable y disponga de `operator==`*

*Calcular la diferencia exige que los dos valores por defecto sean del mismo tipo y que, según el predicado del receptor, sean iguales; si no se cumple, TypeScript lanza `TypeError`, C++ lanza `std::invalid_argument` y Rust entra en pánico*

*Fuera de rango, o al medir un dígito significativo en un vector vacío, TypeScript lanza `TypeError` / `RangeError`, C++ lanza `std::out_of_range` y Rust entra en pánico*

## Licencia

MIT
