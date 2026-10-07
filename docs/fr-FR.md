[English](../README.md) · [Deutsch](de-DE.md) · [Español](es-ES.md) · **Français** · [Italiano](it-IT.md) · [日本語](ja-JP.md) · [한국어](ko-KR.md) · [Русский](ru-RU.md) · [Tiếng Việt](vi-VN.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md)

# sparse-vector

## Aperçu

- **Ce qu'est un vecteur creux**

	Un vecteur creux n'est pas un vecteur mathématique. Il se lit plutôt comme un nombre écrit à la main, où l'« indice » s'étend de l'infini positif à l'infini négatif. Aucun langage n'a d'infini, bien sûr : c'est un `number` en TypeScript, un `int64_t` en C++ et un `i64` en Rust.

	Comme ce n'est pas un vecteur, il ne fait pas d'arithmétique.

- **Comment fonctionne un vecteur creux**

	On demande la valeur à n'importe quel poids de position et l'on obtient toujours une réponse : comment est-ce possible ?

	Seules les positions différentes de la valeur par défaut sont stockées : la valeur par défaut elle-même est stockée en plus. Lorsqu'on consulte une position où rien n'a été stocké, le vecteur renvoie la valeur par défaut.

## Bibliothèques

| Langage | Paquet | Version | État | README |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 3.0.0 | publié | [`typescript/`](../typescript/) |
| C++ | `sparse-vector` | 3.0.0 | publié | [`c++/`](../c++/) |
| Rust | `sparse-vector-rs` | 3.0.0 | publié | [`rust/`](../rust/) |

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

## Sémantique

### Le vecteur

- Ni un vecteur mathématique, ni un tableau informatique, mais la structure de données particulière que fournit cette bibliothèque.

### Le caractère creux

- La capacité du vecteur dépasse de loin le nombre de ses éléments : certains poids de position n'ont jamais reçu de donnée explicite.

### L'indice

- L'indice est l'ensemble des entiers, positifs ou négatifs ; il représente quelque chose comme les unités ou les dizaines.

### La valeur

- Ce que l'on veut réellement stocker, l'équivalent du chiffre des centaines ou des milliers — à ceci près que le type n'est pas forcément un nombre. Ce peut être n'importe quoi.

### L'élément

- Un indice plus une valeur : l'objet ainsi formé s'appelle un élément, et ce sont les éléments que le vecteur stocke réellement.

### La valeur par défaut

- Là où un vecteur creux n'a rien stocké explicitement, c'est la valeur par défaut. C'est comme lorsqu'on écrit un nombre : on laisse tomber les zéros, on écrit 1 plutôt que 0001.000, n'est-ce pas ?

- Une fois le vecteur construit, la valeur par défaut peut être remplacée. Étrange, on ne sait trop à quoi cela sert, mais la possibilité est là.

### Mémoire minimale, maintenue automatiquement

- Remplacer la valeur par défaut écarte aussitôt les éléments qui lui sont égaux.

- Remplacer le prédicat d'égalité fait de même.

- Écrire la valeur par défaut à un poids de position revient à effacer ce qui s'y trouvait.

### L'égalité

- Une valeur est-elle égale à la valeur par défaut ? Selon la méthode de comparaison habituelle de chaque langage.
	- TypeScript : `===`.
	- C++ : `operator==`.
	- Rust : `PartialEq`.

- Un prédicat peut aussi être fourni au vecteur : il reçoit deux paramètres (la valeur à comparer et la valeur par défaut courante), renvoie un booléen et remplace la comparaison habituelle.

- Comparer deux vecteurs, ce sont deux opérations nommées, distinctes de l'élagage : `isEqualTo` / `is_equal_to` détermine si deux vecteurs sont égaux, `differences` énumère les éléments par lesquels le récepteur diffère de l'autre. Toutes deux **décident d'après le prédicat du récepteur**, si bien que, quand les deux prédicats diffèrent, `a.isEqualTo(b)` et `b.isEqualTo(a)` peuvent donner des réponses différentes.

- Attention, la méthode de comparaison de chaque langage a des cas qui peuvent heurter l'intuition.
	- `NaN` n'est pas égal à lui-même.
	- `0`, `'0'`, `false` et `null` sont de quatre types différents, donc ne sont pas égaux.
	- `===` compare les références d'objets, tandis qu'`operator==` et `PartialEq` comparent la structure.

## API

| Rôle | TypeScript | C++ | Rust | Type de retour |
| --- | --- | --- | --- | --- |
| Construction d'un vecteur creux (valeur par défaut omise) | `new SV_vector()` | `SV_vector()` | `SparseVector::default_new()` | Nouveau vecteur |
| Construction d'un vecteur creux | `new SV_vector(defaultValue)` | `SV_vector(defaultValue)` | `SparseVector::new(default)` | Nouveau vecteur |
| Obtenir la valeur par défaut | `getDefaultValue` | `get_default_value()` | `get_default_value()` | valeur en ts, référence en cpp et rust |
| Changer la valeur par défaut | `setDefaultValue = next` | `set_default_value(next)` | `set_default_value(next)` | rien en ts, booléen en cpp et rust |
| Obtenir le prédicat d'égalité | `getEquality` | `get_equality()` | `get_equality()` | prédicat ou `undefined` en ts, prédicat ou vide en cpp et rust |
| Changer le prédicat d'égalité | `setEquality = next` | `set_equality(next)` | `set_equality(next)` | rien en ts, booléen en cpp et rust |
| Obtenir le nombre d'éléments | `getElementAmount` | `get_element_amount()` | `get_element_amount()` | entier |
| Obtenir la dimension significative | `getSignificantDimension` | `get_significant_dimension()` | `get_significant_dimension()` | entier |
| Obtenir la dimension positive | `getPlusDimension` | `get_plus_dimension()` | `get_plus_dimension()` | entier |
| Obtenir la dimension négative | `getMinusDimension` | `get_minus_dimension()` | `get_minus_dimension()` | entier |
| Obtenir la valeur à un indice | `get(index)` | `get(index)` | `get(index)` | le type de la valeur |
| Écrire la valeur à un indice | `set(index, value)` | `set(index, value)` | `set(index, value)` | le vecteur lui-même |
| Réinitialiser la valeur à un indice | `resetValue(index)` | `reset_value(index)` | `reset_value(index)` | booléen |
| Réinitialiser tout le vecteur, sans toucher à la valeur par défaut | `resetVector()` | `reset_vector()` | `reset_vector()` | booléen |
| Obtenir tous les éléments, indice décroissant | `elements()` | `elements()` | `elements()` | tableau d'éléments |
| Obtenir tous les éléments, indice croissant | `invertedElements()` | `inverted_elements()` | `inverted_elements()` | tableau d'éléments |
| Obtenir tous les indices, décroissant | `indexes()` | `indexes()` | `indexes()` | tableau d'indices |
| Obtenir tous les indices, croissant | `invertedIndexes()` | `inverted_indexes()` | `inverted_indexes()` | tableau d'indices |
| Obtenir toutes les valeurs, décroissant | `values()` | `values()` | `values()` | tableau de valeurs |
| Obtenir toutes les valeurs, croissant | `invertedValues()` | `inverted_values()` | `inverted_values()` | tableau de valeurs |
| Obtenir le (n+1)-ième élément depuis la gauche | `element(n)` | `element(n)` | `element(n)` | élément |
| L'indice de cet élément | `elementIndex(n)` | `element_index(n)` | `element_index(n)` | indice |
| La valeur de cet élément | `elementValue(n)` | `element_value(n)` | `element_value(n)` | valeur |
| Obtenir le (n+1)-ième élément depuis la droite | `invertedElement(n)` | `inverted_element(n)` | `inverted_element(n)` | élément |
| L'indice de cet élément | `invertedElementIndex(n)` | `inverted_element_index(n)` | `inverted_element_index(n)` | indice |
| La valeur de cet élément | `invertedElementValue(n)` | `inverted_element_value(n)` | `inverted_element_value(n)` | valeur |
| Obtenir le (n+1)-ième chiffre significatif en partant de la gauche | `leftSignificantValue(n)` | `left_significant_value(n)` | `left_significant_value(n)` | valeur |
| Obtenir le (n+1)-ième chiffre significatif en partant de la droite | `rightSignificantValue(n)` | `right_significant_value(n)` | `right_significant_value(n)` | valeur |
| Itération | `[Symbol.iterator]()` | `begin()` / `end()` | `iter()` | itérateur (prête les éléments par indice décroissant) |
| Copie | `clone()` | construction par copie | `clone()` | nouveau vecteur |
| Construction en bloc | `SV_vector.fromElements(elements, defaultValue?)` | `SV_vector::from_elements(...)` | `SparseVector::from_elements(elements, default)` | nouveau vecteur |
| Déterminer si deux vecteurs sont égaux | `isEqualTo(other)` | `is_equal_to(other)` | `is_equal_to(other)` | booléen |
| Énumérer les éléments qui diffèrent de l'autre vecteur | `differences(other)` | `differences(other)` | `differences(other)` | tableau d'éléments |

*En Rust, obtenir la valeur et énumérer exigent `T: Clone`, tandis qu'écrire, réinitialiser ou changer la valeur par défaut exige `T: PartialEq` ; juger deux vecteurs égaux exige lui aussi `T: PartialEq`, et `differences` exige en plus `T: Clone` ; en C++, il faut que le type soit copiable et dispose d'un `operator==`*

*`differences` suppose que les deux valeurs par défaut soient du même type et égales selon le prédicat du récepteur ; sinon, TypeScript lève `TypeError`, C++ lève `std::invalid_argument` et Rust panique*

*Hors limites, ou pour mesurer un chiffre significatif sur un vecteur vide, TypeScript lève `TypeError` / `RangeError`, C++ lève `std::out_of_range` et Rust panique*

## Licence

MIT
