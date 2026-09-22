[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · **Español** · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

Un vector disperso: una correspondencia de índices enteros a valores arbitrarios que solo almacena las posiciones distintas de un valor por defecto

## La idea

Los índices pueden ser negativos y no hace falta que sean contiguos

Así, un vector con solo tres entradas ocupa solo tres entradas — tanto si esos tres índices son `0, 1, 2` como si son `-10^9, 0, 10^9`

Esta biblioteca deliberadamente **no** es un vector en el sentido matemático. No incorpora ninguna aritmética: es una estructura de datos, y nada más

## Implementaciones

El mismo tipo se ofrece en tres lenguajes. Todos comparten las reglas de más abajo; lo único que cambia es cómo se escribe

| Lenguaje | Paquete | Versión | Directorio | Estado |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 1.1.0 | [`typescript/`](../typescript/) | publicado |
| C++ | `sparse-vector` | 1.1.0 | [`c++/`](../c++/) | publicado |
| Rust | `sparse-vector-rs` | 1.1.0 | [`rust/`](../rust/) | publicado |

Cada paquete se numera de forma independiente; el repositorio en su conjunto está en la versión 1.0.0. Ahora mismo los tres implementan la misma semántica

## Instalación

**TypeScript**

```sh
npm install @calbona/sparse-vector
```

**C++** — Todavía sin publicar en vcpkg ni Conan. Solo dos cabeceras y nada que enlazar, así que basta con apuntar CMake al repositorio:

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

Un checkout junto a tu proyecto funciona igual, con `add_subdirectory(path/to/sparse-vector/c++)`. Un prefijo de instalación también exporta un paquete CMake, así que `find_package(sparse-vector)` funciona igualmente

**Rust** — ojo, el crate que se importa es `sparse_vector`, no el nombre del paquete:

```sh
cargo add sparse-vector-rs
```

El directorio de cada implementación tiene su propio README con el uso y la referencia de la API de ese lenguaje. Esta página define la semántica que comparten, para no tener que repetirla tres veces

## Semántica común a todas las implementaciones

### El valor por defecto

Un vector se crea con un valor por defecto: el valor que se informa para toda posición que no tenga una entrada explícita. Si no se indica otro, es el número `0`

El valor por defecto se puede sustituir después. Al sustituirlo se descartan de inmediato todas las entradas que resulten iguales al nuevo valor por defecto

Como toda posición tiene un valor definido, la lectura es total. Cualquier entero —almacenado o no, dentro del rango o muy fuera de él— devuelve un valor en lugar de lanzar un error

### Una entrada igual al valor por defecto nunca se conserva

Escribir el valor por defecto en una posición equivale a eliminar lo que hubiera ahí. Esto es lo que mantiene dispersa la estructura: la memoria es O(k) respecto al número de entradas que realmente difieren del valor por defecto, por muy separados que estén los índices o por muy negativos que sean

### El descarte usa la igualdad propia de cada lenguaje

Una entrada se descarta cuando compara igual al nuevo valor por defecto, usando la igualdad corriente del lenguaje: `===` en TypeScript, `operator==` en C++, `PartialEq` en Rust

Para números y cadenas los tres coinciden exactamente, incluidos los casos incómodos:

- `-0.0` es igual a `0.0`, así que un `-0.0` almacenado se descarta cuando el valor por defecto es `0.0`
- `NaN` nunca es igual a sí mismo, así que un `NaN` almacenado se conserva aunque el propio valor por defecto sea `NaN`

En TypeScript, `0`, `'0'`, `false` y `null` son cuatro valores de cuatro tipos, y solo una coincidencia exacta descarta una entrada. Un vector con tipos estáticos solo puede contener un único `T`, así que ese conjunto concreto no puede darse en C++ ni en Rust; pero la regla que ilustra —que la igualdad es exacta y no coercitiva— vale en los tres

Con los objetos sí que se separan de verdad, y este es el único punto donde la *identidad* de un valor se hace visible. El `===` de TypeScript compara referencias a objetos; `operator==` y `PartialEq` suelen ser estructurales. Dos objetos distintos con el mismo contenido son un valor en TypeScript y dos en C++ y Rust, de modo que una entrada que contenga un objeto igual pero distinto la conserva el primero y la descartan los otros dos. Cuando lo que quieres es la identidad, incorpórala a la igualdad del propio tipo: un tipo puntero lo consigue sin más, ya que el `operator==` de `std::shared_ptr` compara punteros, y un `Rc<T>` puede envolverse en un newtype que compare con `Rc::ptr_eq`. Los README de C++ y de Rust dan cada uno esa receta

### Serialización

Una entrada es un objeto plano con exactamente dos claves:

| Clave | Tipo | Significado |
| --- | --- | --- |
| `index` | entero | la posición, negativa o no |
| `value` | cualquier cosa | el valor almacenado ahí |

Un vector se serializa únicamente a sus entradas, en orden ascendente de índice. El valor por defecto no forma parte de esta estructura, así que al ir y volver hay que llevarlo aparte

## TypeScript

Publicado como `@calbona/sparse-vector`. Uso y referencia de la API en [`typescript/README.md`](../typescript/README.md)

## C++

Implementación de C++17 en solo cabeceras que se toma del repositorio; todavía no está en vcpkg ni en Conan. Uso y referencia de la API en [`c++/README.md`](../c++/README.md)

## Rust

Publicado como `sparse-vector-rs`. Uso y referencia de la API en [`rust/README.md`](../rust/README.md)

## Licencia

MIT
