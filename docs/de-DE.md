[English](../README.md) · **Deutsch** · [Español](es-ES.md) · [Français](fr-FR.md) · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Überblick

- **Ein dünnbesetzter Vektor ist kein Vektor**

	Sein „Index“ beginnt nicht bei null und läuft nicht von links nach rechts, sondern funktioniert eher wie eine von Hand geschriebene Zahl: Der Index vertritt das Stellenwertgewicht, sodass er von plus unendlich bis minus unendlich reicht. Natürlich kennt keine Sprache Unendlichkeiten: In TypeScript ist es ein `number`, in C++ ein `int64_t` und in Rust ein `i64`.

	Und da er kein mathematischer Vektor ist, trägt er keine Arithmetik. Insofern ist er auch ein Wörterbuch, und jede Position nimmt tatsächlich Daten beliebigen Typs auf.

- **Wie ein dünnbesetzter Vektor entsteht**

	Gespeichert werden nur die Positionen, die vom Standardwert abweichen.

	Man nimmt also mehrere Objekte namens „Einträge“ — je ein Index mit einem vom Standardwert abweichenden Wert —, reiht sie zu einer Liste auf, fügt den Standardwert hinzu, und schon steht er da, der dünnbesetzte Vektor. Wo die Indizes auch liegen: k Einträge bedeuten O(k) Speicher.

## Bibliotheken

| Sprache | Paket | Version | Status | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | veröffentlicht | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | veröffentlicht | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | veröffentlicht | [`rust/`](../rust/) |

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

### Der Standardwert einer leeren Position

- Ein dünnbesetzter Vektor wird mit einem Standardwert angelegt

- Dieser Standardwert lässt sich später ersetzen

- Jede Position hat einen definierten Wert, das Lesen hat also stets eine Antwort: Jede ganze Zahl liefert einen Wert

### Ein Eintrag gleich dem Standardwert wird nie behalten

- Den Standardwert in eine Position zu schreiben, löscht alles, was dort stand

- Beim Ersetzen des Standardwerts werden die ihm gleichen Einträge sofort entfernt

- Genau dieses Prinzip hält die Struktur dünn besetzt

### Gleichheit

- Ob ein Eintrag dem Standardwert gleich ist, richtet sich nach der üblichen Praxis der jeweiligen Sprache: `===` in TypeScript, `operator==` in C++, `PartialEq` in Rust

- Die heiklen Fälle:
	- `-0.0` ist gleich `0.0`
	- `NaN` ist nicht gleich sich selbst
	- `0`, `'0'`, `false` und `null` sind vier Werte aus vier Typen
	- `===` vergleicht Objektreferenzen, `operator==` und `PartialEq` vergleichen dagegen die Struktur: Zwei verschiedene Objekte mit gleichem Inhalt sind in TypeScript ein Wert und in C++ und Rust zwei, die erste Umsetzung behält sie also und die beiden anderen verwerfen sie

- Wenn Identität gefragt ist, baut man sie in die Gleichheit des Typs selbst ein; ein Zeigertyp liefert sie direkt: Der `operator==` von `std::shared_ptr` vergleicht Zeiger, und ein `Rc<T>` lässt sich in einen Newtype packen, der mit `Rc::ptr_eq` vergleicht. Die READMEs zu C++ und Rust geben das Rezept

## API

| Zweck | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| Konstruktion eines dünnbesetzten Vektors (Standardwert weggelassen) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (nur `f64`) / `SparseVector::default()` |
| Konstruktion eines dünnbesetzten Vektors | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| Standardwert lesen | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| Standardwert ändern | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| Anzahl der Einträge lesen | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| Signifikante Dimension lesen | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| Positive Dimension lesen | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| Negative Dimension lesen | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| Wert an einem Index lesen | `get(index)` | `get(index)` | `get(index)` |
| Wert an einem Index schreiben | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| Wert an einem Index zurücksetzen | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| Den ganzen Vektor zurücksetzen | `resetVector()` | `reset_vector()` | `reset_vector()` |
| Alle Einträge, Index absteigend | `elements()` | `elements()` | `elements()` |
| Alle Einträge, Index aufsteigend | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| Alle belegten Indizes, absteigend | `indexes()` | `indexes()` | `indexes()` |
| Alle belegten Indizes, aufsteigend | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| Alle belegten Werte, absteigend | `values()` | `values()` | `values()` |
| Alle belegten Werte, aufsteigend | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| Den (n+1)-ten Eintrag von links | `element(n)` | `element(n)` | `element(n)` |
| Dessen Index | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| Dessen Wert | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| Den (n+1)-ten Eintrag von rechts | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| Dessen Index | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| Dessen Wert | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| Die (n+1)-te signifikante Stelle von links | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| Die (n+1)-te signifikante Stelle von rechts | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| Iteration | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| Kopie | `clone()` | Kopierkonstruktor | `clone()` |
| Massenkonstruktion | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*In Rust verlangen Lesen und Auflisten `T: Clone`, Schreiben, Zurücksetzen und Ändern des Standardwerts `T: PartialEq`; in C++ entsprechend Kopierbarkeit und `operator==`*

*Bei einem Index außerhalb des Bereichs oder beim Messen einer signifikanten Stelle auf einem leeren Vektor wirft TypeScript `TypeError` / `RangeError`, C++ wirft `std::out_of_range` und Rust gerät in Panik*

## Lizenz

MIT
