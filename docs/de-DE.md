[English](../README.md) · **Deutsch** · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Überblick

- **Was ein dünnbesetzter Vektor ist**

	Ein dünnbesetzter Vektor ist kein mathematischer Vektor. Er liest sich wie eine von Hand geschriebene Zahl, sein „Index“ reicht von plus unendlich bis minus unendlich. Natürlich kennt keine Sprache Unendlichkeiten: In TypeScript ist es ein `number`, in C++ ein `int64_t` und in Rust ein `i64`.

	Da er kein Vektor ist, kennt er auch keine Arithmetik.

- **Wie ein dünnbesetzter Vektor funktioniert**

	Wie kommt es, dass man nach dem Wert an einem beliebigen Stellenwertgewicht fragen kann und stets eine Antwort erhält? Gespeichert werden nur die Positionen, die vom Standardwert abweichen, dazu der Standardwert selbst. Fragt man eine Position ab, an der nichts gespeichert wurde, gibt der Vektor den Standardwert zurück.

## Bibliotheken

| Sprache | Paket | Version | Status | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | veröffentlicht | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | veröffentlicht | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | veröffentlicht | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*noch nicht in vcpkg oder Conan*

CMake einfach auf das Repository zeigen lassen

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

Eine Arbeitskopie neben dem eigenen Projekt funktioniert genauso, mit `add_subdirectory(path/to/sparse-vector/c++)`
Ein Installationspräfix exportiert ebenfalls ein CMake-Paket, `find_package(sparse-vector)` funktioniert also auch

### Rust

```sh
cargo add sparse-vector-rs
```

## Semantik

### Vektor

- Weder ein mathematischer Vektor noch ein Array, sondern die besondere Datenstruktur, die diese Bibliothek bereitstellt.

### Dünnbesetzt

- Die Kapazität übersteigt die Anzahl der Elemente bei Weitem: Manche Stellenwertgewichte wurden nie ausdrücklich gespeichert.

### Index

- Alle ganzen Zahlen, positiv oder negativ. Er steht für etwas wie die Einer- oder die Zehnerstelle.

### Wert

- Das, was eigentlich gespeichert werden soll, das Gegenstück zur Ziffer an der Hunderter- oder Tausenderstelle — nur muss der Typ keine Zahl sein. Er kann alles sein.

### Element

- Ein Index plus ein Wert. Dieses Objekt heißt Element, und Elemente sind das, was der Vektor wirklich speichert.

### Standardwert

- Jede Position, an der nichts ausdrücklich gespeichert wurde, trägt den Standardwert. Vergleichbar mit dem Schreiben einer Zahl: Die Nullen lässt man weg, man schreibt 1 statt 0001.000, oder?

- Ist der Vektor einmal gebaut, lässt sich der Standardwert ersetzen. Seltsam — wer weiß, wofür —, aber die Möglichkeit besteht.

### Automatisch minimaler Speicher

- Beim Ersetzen des Standardwerts werden sofort alle Elemente entfernt, deren Wert ihm gleich ist.

- Beim Ersetzen des Gleichheitsprädikats geschieht dasselbe.

- Wird an eine Position der Standardwert geschrieben, löscht das den bisherigen Inhalt.

### Gleichheit

- Ist ein Wert gleich dem Standardwert? Nach der üblichen Prüfung der jeweiligen Sprache.
	- TypeScript: `===`.
	- C++: `operator==`.
	- Rust: `PartialEq`.

- Man kann dem Vektor auch ein eigenes Prädikat mitgeben: Es nimmt den zu vergleichenden Wert und den aktuellen Standardwert entgegen und liefert einen booleschen Wert; es tritt an die Stelle der üblichen Prüfung.

- Das Vergleichen zweier Vektoren umfasst zwei benannte Vorgänge und ist nicht dasselbe wie das Ausdünnen: `isEqualTo` / `is_equal_to` entscheidet, ob zwei Vektoren gleich sind, `differences` listet die Elemente auf, in denen der Empfänger vom anderen abweicht. Beide **richten sich nach dem Prädikat des Empfängers**; tragen die beiden verschiedene Prädikate, können `a.isEqualTo(b)` und `b.isEqualTo(a)` unterschiedliche Antworten geben.

- Die üblichen Gleichheitsprüfungen der einzelnen Sprachen kennen einige Fälle, die der Intuition widersprechen.
	- `NaN` ist nicht gleich sich selbst.
	- `0`, `'0'`, `false` und `null` haben vier verschiedene Typen, sind also nicht gleich.
	- `===` vergleicht Objektreferenzen, `operator==` und `PartialEq` vergleichen dagegen die Struktur.

## API

| Zweck | TypeScript | C++ | Rust | Rückgabetyp |
| --- | --- | --- | --- | --- |
| Konstruktion eines dünnbesetzten Vektors (Standardwert weggelassen) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | Neuer Vektor |
| Konstruktion eines dünnbesetzten Vektors | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | Neuer Vektor |
| Standardwert lesen | `getDefaultValue` | `get_default_value()` | `get_default_value()` | ts Wert, cpp/rust Referenz |
| Standardwert ändern | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | ts nichts, cpp/rust boolesch |
| Gleichheitsprädikat lesen | `getEquality` | `get_equality()` | `get_equality()` | ts Prädikat oder `undefined`, cpp/rust Prädikat oder leer |
| Gleichheitsprädikat ändern | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | ts nichts, cpp/rust boolesch |
| Anzahl der Elemente lesen | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | Ganzzahl |
| Signifikante Dimension lesen | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | Ganzzahl |
| Positive Dimension lesen | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | Ganzzahl |
| Negative Dimension lesen | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | Ganzzahl |
| Wert an einem Index lesen | `get(index)` | `get(index)` | `get(index)` | Typ des Werts |
| Wert an einem Index schreiben | `set(index, value)` | `set(index, value)` | `set(index, value)` | Der Vektor selbst |
| Wert an einem Index zurücksetzen | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | Boolesch |
| Den ganzen Vektor zurücksetzen, ohne den Standardwert | `resetVector()` | `reset_vector()` | `reset_vector()` | Boolesch |
| Alle Elemente, Index absteigend | `elements()` | `elements()` | `elements()` | Element-Array |
| Alle Elemente, Index aufsteigend | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | Element-Array |
| Alle belegten Indizes, absteigend | `indexes()` | `indexes()` | `indexes()` | Index-Array |
| Alle belegten Indizes, aufsteigend | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | Index-Array |
| Alle belegten Werte, absteigend | `values()` | `values()` | `values()` | Wert-Array |
| Alle belegten Werte, aufsteigend | `invertedValues()` | `inverted_values()` | `inverted_values()` | Wert-Array |
| Das (n+1)-te Element von links | `element(n)` | `element(n)` | `element(n)` | Element |
| Dessen Index | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | Index |
| Dessen Wert | `elementValue(n)` | `element_value(n)` | `element_value(n)` | Wert |
| Das (n+1)-te Element von rechts | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | Element |
| Dessen Index | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | Index |
| Dessen Wert | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | Wert |
| Die (n+1)-te signifikante Stelle von links | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | Wert |
| Die (n+1)-te signifikante Stelle von rechts | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | Wert |
| Iteration | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | Iterator (leiht Elemente nach Index absteigend aus) |
| Kopie | `clone()` | Kopierkonstruktor | `clone()` | Neuer Vektor |
| Massenkonstruktion | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | Neuer Vektor |
| Gleichheit zweier Vektoren prüfen | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | Boolesch |
| Vom anderen Vektor abweichende Elemente auflisten | `differences(other)` | `differences(other)` | `differences(other)` | Element-Array |

*In Rust verlangen Lesen und Auflisten `T: Clone`, Schreiben, Zurücksetzen und Ändern des Standardwerts `T: PartialEq`; das Prüfen zweier Vektoren auf Gleichheit verlangt ebenfalls `T: PartialEq`, das Bilden der Differenz zusätzlich `T: Clone`; in C++ Kopierbarkeit und `operator==`*

*Voraussetzung für die Differenz ist, dass beide Standardwerte denselben Typ haben und nach dem Prädikat des Empfängers gleich sind; andernfalls wirft TypeScript `TypeError`, C++ wirft `std::invalid_argument`, Rust gerät in Panik*

*Bei einem Index außerhalb des Bereichs oder beim Messen einer signifikanten Stelle auf einem leeren Vektor wirft TypeScript `TypeError` / `RangeError`, C++ wirft `std::out_of_range` und Rust gerät in Panik*

## Lizenz

MIT
