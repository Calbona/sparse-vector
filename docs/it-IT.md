[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · [Français](fr-FR.md) · **Italiano** · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Panoramica

- **Che cos'è un vettore sparso**

	Un vettore sparso non è un vettore matematico: si legge piuttosto come un numero scritto a mano, con l'«indice» che si estende dall'infinito positivo giù fino a quello negativo. Naturalmente, nessun linguaggio conosce l'infinito: in TypeScript è un `number`, in C++ un `int64_t` e in Rust un `i64`.

	E, non essendo un vettore, non vi si applica alcuna aritmetica.

- **Come funziona un vettore sparso**

	Si chiede il valore di un qualsiasi peso posizionale e si ottiene una risposta: come è possibile?

	Si memorizzano soltanto le posizioni diverse dal valore predefinito, più il valore predefinito stesso. E quando si interroga una posizione in cui non è stato memorizzato nulla, il vettore restituisce il valore predefinito.

## Librerie

| Linguaggio | Pacchetto | Versione | Stato | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | pubblicato | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | pubblicato | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | pubblicato | [`rust/`](../rust/) |

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

## Semantica

### Vettore

- Non un vettore matematico, né un array del computer, ma la struttura dati particolare che questa libreria fornisce.

### Sparso

- La capacità supera di gran lunga il numero di elementi: alcuni pesi posizionali non sono mai stati memorizzati in modo esplicito.

### Indice

- Ogni numero intero, positivo o negativo: sta per qualcosa come la cifra delle unità o delle decine.

### Valore

- Ciò che si vuole davvero memorizzare, l'analogo della cifra delle centinaia o delle migliaia; solo che il tipo non è per forza un numero, può essere qualsiasi cosa.

### Elemento

- Un indice più un valore: l'oggetto che ne risulta è un elemento, ed è ciò che il vettore memorizza davvero.

### Valore predefinito

- Le posizioni del vettore sparso in cui non è stato memorizzato nulla in modo esplicito valgono il valore predefinito; si può pensare agli 0 che si omettono scrivendo un numero: di solito si scrive 1, non 0001.000, no?

- Una volta costruito il vettore, il valore predefinito si può sostituire. Chissà a cosa può servire, ma la possibilità c'è.

### Memoria minima mantenuta automaticamente

- Sostituendo il valore predefinito si scartano subito gli elementi uguali a esso.

- Sostituendo il criterio di uguaglianza, lo stesso.

- Scrivere il valore predefinito in una posizione cancella ciò che vi si trovava.

### L'uguaglianza

- Un valore è uguale al valore predefinito? Secondo il metodo consueto di ciascun linguaggio.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- Si può anche assegnare al vettore un criterio di uguaglianza: riceve il valore da confrontare e il valore predefinito corrente, restituisce un booleano e sostituisce il metodo consueto.

- Il confronto tra due vettori consiste in due operazioni con un nome, distinte dalla potatura: `isEqualTo` / `is_equal_to` stabilisce se due vettori sono uguali, mentre `differences` elenca gli elementi in cui il ricevente differisce dall'altro. Entrambe **seguono il criterio del ricevente**, perciò, quando i due criteri differiscono, `a.isEqualTo(b)` e `b.isEqualTo(a)` possono dare risposte diverse.

- Questi metodi consueti hanno alcuni casi che possono apparire controintuitivi.
	- `NaN` non è uguale a se stesso.
	- `0`, `'0'`, `false` e `null` sono di quattro tipi, quindi non sono uguali.
	- `===` confronta i riferimenti agli oggetti, mentre `operator==` e `PartialEq` confrontano la struttura.

## API

| Scopo | TypeScript | C++ | Rust | Tipo restituito |
| --- | --- | --- | --- | --- |
| Costruzione di un vettore (valore predefinito omesso) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | Nuovo vettore |
| Costruzione di un vettore | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | Nuovo vettore |
| Accedere al valore predefinito | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts valore, cpp, rust riferimento |
| Aggiornare il valore predefinito | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts niente, cpp, rust booleano |
| Accedere al criterio di uguaglianza | `getEquality` | `get_equality()` | `get_equality()` | ts il criterio o `undefined`, cpp, rust il criterio o vuoto |
| Aggiornare il criterio di uguaglianza | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts niente, cpp, rust booleano |
| Ottenere il numero di elementi | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | intero |
| Ottenere la dimensione significativa | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | intero |
| Ottenere la dimensione positiva | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | intero |
| Ottenere la dimensione negativa | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | intero |
| Accedere al valore a un indice | `get(index)` | `get(index)` | `get(index)` | il tipo del valore |
| Aggiornare il valore a un indice | `set(index, value)` | `set(index, value)` | `set(index, value)` | il vettore stesso |
| Azzerare il valore a un indice | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | booleano |
| Azzerare l'intero vettore, senza azzerare il valore predefinito | `resetVector()` | `reset_vector()` | `reset_vector()` | booleano |
| Tutti gli elementi, indice decrescente | `elements()` | `elements()` | `elements()` | array di elementi |
| Tutti gli elementi, indice crescente | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | array di elementi |
| Tutti gli indici non vuoti, decrescente | `indexes()` | `indexes()` | `indexes()` | array di indici |
| Tutti gli indici non vuoti, crescente | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | array di indici |
| Tutti i valori non vuoti, decrescente | `values()` | `values()` | `values()` | array di valori |
| Tutti i valori non vuoti, crescente | `invertedValues()` | `inverted_values()` | `inverted_values()` | array di valori |
| Il (n+1)-esimo elemento da sinistra | `element(n)` | `element(n)` | `element(n)` | elemento |
| Il suo indice | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | indice |
| Il suo valore | `elementValue(n)` | `element_value(n)` | `element_value(n)` | valore |
| Il (n+1)-esimo elemento da destra | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | elemento |
| Il suo indice | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | indice |
| Il suo valore | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | valore |
| La (n+1)-esima cifra significativa da sinistra | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | valore |
| La (n+1)-esima cifra significativa da destra | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | valore |
| Iterazione | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | iteratore (presta gli elementi in ordine decrescente di indice) |
| Copia | `clone()` | costruttore di copia | `clone()` | Nuovo vettore |
| Costruzione in blocco | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | Nuovo vettore |
| Stabilire se due vettori sono uguali | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | booleano |
| Elencare gli elementi diversi da un altro vettore | `differences(other)` | `differences(other)` | `differences(other)` | array di elementi |

*In Rust leggere ed elencare richiedono `T: Clone`, mentre scrivere, azzerare o cambiare il valore predefinito richiede `T: PartialEq`; stabilire se due vettori sono uguali richiede anch'esso `T: PartialEq`, e la differenza richiede in più `T: Clone`; in C++, di conseguenza, servono la copiabilità e `operator==`*

*La differenza presuppone che i due valori predefiniti siano dello stesso tipo e uguali secondo il criterio del ricevente; in caso contrario TypeScript solleva `TypeError`, C++ solleva `std::invalid_argument` e Rust va in panico*

*Fuori dai limiti, o misurando una cifra significativa su un vettore vuoto, TypeScript solleva `TypeError` / `RangeError`, C++ solleva `std::out_of_range` e Rust va in panico*

## Licenza

MIT
