[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · **Italiano** · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Panoramica

- **Un vettore sparso non è un vettore**

	Il suo «indice» non parte da zero e non scorre da sinistra a destra: funziona piuttosto come un numero scritto a mano, dove l'indice fa da peso posizionale, così si estende dall'infinito positivo a quello negativo. Nessun linguaggio ha infiniti, naturalmente: in TypeScript è un `number`, in C++ un `int64_t` e in Rust un `i64`.

	E poiché non è un vettore matematico, non comporta alcuna aritmetica. In questo senso è anche un dizionario, e ogni posizione accoglie davvero dati di qualsiasi tipo.

- **Come è fatto un vettore sparso**

	Si memorizzano solo le posizioni diverse dal valore predefinito.

	Si prendono quindi diversi oggetti detti «voci» — ognuno un indice accanto a un valore diverso da quello predefinito —, li si dispone in una lista, si aggiunge il valore predefinito ed eccolo lì: il vettore sparso. Ovunque cadano gli indici, k voci significano O(k) di memoria.

## Librerie

| Linguaggio | Pacchetto | Versione | Stato | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | pubblicato | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | pubblicato | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | pubblicato | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*non ancora su vcpkg né su Conan*

Basta puntare CMake al repository

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

Un checkout accanto al proprio progetto funziona allo stesso modo, con `add_subdirectory(path/to/sparse-vector/c++)`
Anche un prefisso di installazione esporta un pacchetto CMake, quindi `find_package(sparse-vector)` funziona

### Rust

```sh
cargo add sparse-vector-rs
```

## Dettagli della semantica

### Il valore predefinito di una posizione vuota

- Un vettore sparso viene creato con un valore predefinito

- Quel valore predefinito si può sostituire in seguito

- Ogni posizione ha un valore definito, quindi la lettura ha sempre una risposta: qualsiasi intero restituisce un valore

### Una voce uguale al valore predefinito non viene mai conservata

- Scrivere il valore predefinito in una posizione equivale a cancellare ciò che vi si trovava

- Sostituendo il valore predefinito si scartano subito le voci uguali a esso

- È questo il principio che tiene sparsa la struttura

### L'uguaglianza

- Se una voce sia uguale al valore predefinito segue la pratica consueta di ciascun linguaggio: `===` in TypeScript, `operator==` in C++, `PartialEq` in Rust

- I casi scomodi:
	- `-0.0` è uguale a `0.0`
	- `NaN` non è uguale a se stesso
	- `0`, `'0'`, `false` e `null` sono quattro valori di quattro tipi
	- `===` confronta i riferimenti agli oggetti, mentre `operator==` e `PartialEq` confrontano la struttura: due oggetti distinti con lo stesso contenuto sono un valore in TypeScript e due in C++ e Rust, quindi il primo li conserva e gli altri due li scartano

- Quando serve l'identità, la si integra nell'uguaglianza del tipo stesso; un tipo puntatore la offre direttamente: l'`operator==` di `std::shared_ptr` confronta i puntatori, e un `Rc<T>` si può avvolgere in un newtype che confronta con `Rc::ptr_eq`. I README di C++ e Rust ne danno la ricetta

## API

| Scopo | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| Costruzione di un vettore sparso (valore predefinito omesso) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (solo `f64`) / `SparseVector::default()` |
| Costruzione di un vettore sparso | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| Ottenere il valore predefinito | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| Cambiare il valore predefinito | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| Ottenere il numero di voci | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| Ottenere la dimensione significativa | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| Ottenere la dimensione positiva | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| Ottenere la dimensione negativa | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| Ottenere il valore a un indice | `get(index)` | `get(index)` | `get(index)` |
| Scrivere il valore a un indice | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| Azzerare il valore a un indice | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| Azzerare l'intero vettore | `resetVector()` | `reset_vector()` | `reset_vector()` |
| Tutte le voci, indice decrescente | `elements()` | `elements()` | `elements()` |
| Tutte le voci, indice crescente | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| Tutti gli indici non vuoti, decrescente | `indexes()` | `indexes()` | `indexes()` |
| Tutti gli indici non vuoti, crescente | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| Tutti i valori non vuoti, decrescente | `values()` | `values()` | `values()` |
| Tutti i valori non vuoti, crescente | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| La (n+1)-esima voce da sinistra | `element(n)` | `element(n)` | `element(n)` |
| Il suo indice | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| Il suo valore | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| La (n+1)-esima voce da destra | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| Il suo indice | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| Il suo valore | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| La (n+1)-esima cifra significativa da sinistra | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| La (n+1)-esima cifra significativa da destra | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| Iterazione | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| Copia | `clone()` | costruttore di copia | `clone()` |
| Costruzione in blocco | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*In Rust leggere ed elencare richiedono `T: Clone`, mentre scrivere, azzerare o cambiare il valore predefinito richiede `T: PartialEq`; in C++, di conseguenza, servono la copiabilità e `operator==`*

*Fuori dai limiti, o misurando una cifra significativa su un vettore vuoto, TypeScript solleva `TypeError` / `RangeError`, C++ solleva `std::out_of_range` e Rust va in panico*

## Licenza

MIT
