[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · **Français** · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Concept

- **Un vecteur creux n'est pas un vecteur**

	Son « indice » ne commence pas à zéro et ne court pas de gauche à droite : il se lit plutôt comme un nombre écrit à la main, où l'indice fait office de poids de position, si bien qu'il s'étend de l'infini positif à l'infini négatif. Aucun langage n'a d'infini, bien sûr : c'est un `number` en TypeScript, un `int64_t` en C++ et un `i64` en Rust.

	Et comme ce n'est pas un vecteur mathématique, il ne comporte aucune arithmétique. En ce sens, c'est aussi un dictionnaire, et chaque position accepte réellement des données de n'importe quel type.

- **Comment un vecteur creux est fait**

	Seules les positions différentes de la valeur par défaut sont stockées.

	On réunit donc plusieurs objets appelés « entrées » — un indice accompagné d'une valeur différente de la valeur par défaut — pour former une liste, on y ajoute la valeur par défaut, et voilà, le vecteur creux est prêt. Quels que soient les indices, s'il n'y a que k entrées, la mémoire est en O(k).

## Bibliothèques

| Langage | Paquet | Version | État | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 2.0.0 | publié | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 2.0.0 | publié | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 2.0.0 | publié | [`rust/`](../rust/) |

### TypeScript

```sh
npm install @calbona/sparse-vector
```

### C++

*Pas encore publiée sur vcpkg ni Conan*

Il suffit d'indiquer le dépôt à CMake

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

Un checkout à côté de votre projet fonctionne de la même façon, avec `add_subdirectory(path/to/sparse-vector/c++)`
Un préfixe d'installation exporte aussi un paquet CMake, donc `find_package(sparse-vector)` fonctionne également

### Rust

```sh
cargo add sparse-vector-rs
```

## Détails de la sémantique

### La valeur par défaut des positions vides

- Un vecteur creux se crée avec une valeur par défaut

- Cette valeur par défaut peut être remplacée ensuite

- Chaque position a une valeur définie, donc la lecture a toujours une réponse : tout entier renvoie une valeur

### Une entrée égale à la valeur par défaut n'est jamais conservée

- Écrire la valeur par défaut dans une position revient à effacer ce qui s'y trouvait

- Remplacer la valeur par défaut écarte aussitôt les entrées qui lui sont égales

- C'est ce principe qui garde la structure creuse

### L'égalité

- Qu'une entrée soit égale à la valeur par défaut suit la pratique habituelle de chaque langage : `===` en TypeScript, `operator==` en C++, `PartialEq` en Rust

- Les cas délicats :
	- `-0.0` est égal à `0.0`
	- `NaN` n'est pas égal à lui-même
	- `0`, `'0'`, `false` et `null` sont quatre valeurs de quatre types
	- `===` compare les références d'objets, tandis qu'`operator==` et `PartialEq` comparent la structure : deux objets distincts au contenu identique forment une valeur en TypeScript et deux en C++ et Rust, donc le premier les conserve et les deux autres les écartent

- Quand c'est l'identité qu'il vous faut, intégrez-la à l'égalité du type lui-même ; un type pointeur la donne sans détour : l'`operator==` de `std::shared_ptr` compare les pointeurs, et un `Rc<T>` peut être enveloppé dans un newtype comparant avec `Rc::ptr_eq`. Les README C++ et Rust détaillent cela

## API

| Rôle | TypeScript | C++ | Rust |
| --- | --- | --- | --- |
| Construction d'un vecteur creux (valeur par défaut omise) | `new SV_vector()` | `SV_vector()` | `SparseVector::new()` (seulement `f64`) / `SparseVector::default()` |
| Construction d'un vecteur creux | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::with_default(default)` |
| Obtenir la valeur par défaut | `getDefaultValue` | `get_default_value()` | `get_default_value()` |
| Changer la valeur par défaut | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` |
| Obtenir le nombre d'entrées | `getElementAmount` | `get_element_amount()` | `get_element_amount()` |
| Obtenir la dimension significative | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` |
| Obtenir la dimension positive | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` |
| Obtenir la dimension négative | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` |
| Obtenir la valeur à un indice | `get(index)` | `get(index)` | `get(index)` |
| Écrire la valeur à un indice | `set(index, value)` | `set(index, value)` | `set(index, value)` |
| Réinitialiser la valeur à un indice | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` |
| Réinitialiser tout le vecteur | `resetVector()` | `reset_vector()` | `reset_vector()` |
| Obtenir toutes les entrées, indice décroissant | `elements()` | `elements()` | `elements()` |
| Obtenir toutes les entrées, indice croissant | `invertedElements()` | `inverted_elements()` | `inverted_elements()` |
| Obtenir tous les indices non vides, décroissant | `indexes()` | `indexes()` | `indexes()` |
| Obtenir tous les indices non vides, croissant | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` |
| Obtenir toutes les valeurs non vides, décroissant | `values()` | `values()` | `values()` |
| Obtenir toutes les valeurs non vides, croissant | `invertedValues()` | `inverted_values()` | `inverted_values()` |
| Obtenir la (n+1)-ième entrée depuis la gauche | `element(n)` | `element(n)` | `element(n)` |
| L'indice de cette entrée | `elementIndex(n)` | `element_index(n)` | `element_index(n)` |
| La valeur de cette entrée | `elementValue(n)` | `element_value(n)` | `element_value(n)` |
| Obtenir la (n+1)-ième entrée depuis la droite | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` |
| L'indice de cette entrée | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` |
| La valeur de cette entrée | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` |
| Obtenir le (n+1)-ième chiffre significatif en partant de la gauche | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` |
| Obtenir le (n+1)-ième chiffre significatif en partant de la droite | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` |
| Itération | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` |
| Copie | `clone()` | construction par copie | `clone()` |
| Construction en bloc | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` |

*En Rust, obtenir la valeur et énumérer exige `T: Clone`, tandis qu'écrire, réinitialiser ou changer la valeur par défaut exige `T: PartialEq` ; en C++, de même, il faut que le type soit copiable et dispose d'`operator==`*

*Hors limites, ou pour mesurer un chiffre significatif sur un vecteur vide, TypeScript lève `TypeError` / `RangeError`, C++ lève `std::out_of_range` et Rust panique*

## Licence

MIT
