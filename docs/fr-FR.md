[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · [Español](es-ES.md) · **Français** · [Русский](ru-RU.md)

# sparse-vector

Un vecteur creux : une correspondance d'indices entiers vers des valeurs quelconques, qui ne stocke que les positions différentes d'une valeur par défaut

## Concept

Les indices peuvent être négatifs et n'ont pas besoin d'être contigus

Ainsi, un vecteur qui ne contient que trois entrées n'occupe que trois entrées — que ces trois indices soient `0, 1, 2` ou `-10^9, 0, 10^9`

Cette bibliothèque n'est délibérément **pas** un vecteur au sens mathématique. Elle ne porte aucune arithmétique ; c'est une structure de données, et rien d'autre

## Implémentations

Le même type est proposé dans trois langages. Ils partagent toutes les règles ci-dessous — seule l'écriture diffère

| Langage | Paquet | Version | Répertoire | État |
| --- | --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | 1.0.2 | [`typescript/`](../typescript/) | publié |
| C++ | `sparse-vector` | 1.0.0 | [`c++/`](../c++/) | publié |
| Rust | `sparse-vector-rs` | 1.0.0 | [`rust/`](../rust/) | publié |

Chaque paquet est numéroté indépendamment, d'où des numéros différents. Les trois implémentent actuellement la même sémantique

## Installation

**TypeScript**

```sh
npm install @calbona/sparse-vector
```

**C++** — pas encore publiée sur vcpkg ni Conan. Deux en-têtes et rien à lier : il suffit d'indiquer le dépôt à CMake :

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

Un checkout à côté de votre projet fonctionne de la même façon, avec `add_subdirectory(path/to/sparse-vector/c++)`. Un préfixe d'installation exporte aussi un paquet CMake, donc `find_package(sparse-vector)` fonctionne également

**Rust** — à noter que le crate importé est `sparse_vector`, et non le nom du paquet :

```sh
cargo add sparse-vector-rs
```

Chaque répertoire d'implémentation possède son propre README, avec l'utilisation et la référence d'API de ce langage. Cette page définit la sémantique qu'ils ont en commun, pour ne pas avoir à la répéter trois fois

## Sémantique commune à toutes les implémentations

### La valeur par défaut

Un vecteur est créé avec une valeur par défaut : la valeur rapportée pour toute position qui ne porte aucune entrée explicite. Elle vaut par défaut le `0` de type number, sauf si une autre est fournie

La valeur par défaut peut être remplacée plus tard. La remplacer écarte immédiatement toutes les entrées qui sont égales à la nouvelle valeur par défaut

Comme chaque position a une valeur définie, la lecture est totale. Tout entier — stocké ou non, dans les limites ou bien au-delà — renvoie une valeur plutôt que de lever une erreur

### Une entrée égale à la valeur par défaut n'est jamais conservée

Écrire la valeur par défaut dans une position revient à supprimer ce qui s'y trouvait. C'est ce qui garde la structure creuse : la mémoire est en O(k) selon le nombre d'entrées qui diffèrent réellement de la valeur par défaut, quelle que soit la distance entre les indices, et même s'ils sont très négatifs

### L'écartement se fait par l'égalité propre à chaque langage

Une entrée est écartée lorsqu'elle est égale à la nouvelle valeur par défaut, selon l'égalité ordinaire du langage — `===` en TypeScript, `operator==` en C++, `PartialEq` en Rust

Pour les nombres et les chaînes, les trois coïncident exactement, y compris dans les cas délicats :

- `-0.0` est égal à `0.0`, donc un `-0.0` stocké est écarté quand la valeur par défaut est `0.0`
- `NaN` n'est jamais égal à lui-même, donc un `NaN` stocké est conservé même si la valeur par défaut est elle-même `NaN`

En TypeScript, `0`, `'0'`, `false` et `null` sont quatre valeurs de quatre types, et seule une correspondance exacte écarte une entrée. Un vecteur à typage statique ne peut contenir qu'un seul `T` : cet ensemble précis ne peut donc pas se présenter en C++ ni en Rust — mais la règle qu'il illustre, à savoir que l'égalité est exacte et non coercitive, vaut dans les trois

Pour les objets, en revanche, les trois divergent réellement, et c'est le seul endroit où l'*identité* d'une valeur devient visible. Le `===` de TypeScript compare des références d'objets ; `operator==` et `PartialEq` sont le plus souvent structurels. Deux objets distincts au contenu identique forment une seule valeur en TypeScript et deux en C++ et Rust : une entrée contenant un objet égal mais distinct est donc conservée par le premier et écartée par les deux autres. Quand c'est l'identité que vous voulez, intégrez-la à l'égalité du type lui-même — un type pointeur y suffit, puisque l'`operator==` de `std::shared_ptr` compare les pointeurs, et un `Rc<T>` peut être enveloppé dans un newtype comparant avec `Rc::ptr_eq`. Les README C++ et Rust donnent chacun cette recette

### Sérialisation

Une entrée est un objet ordinaire avec exactement deux clés :

| Clé | Type | Signification |
| --- | --- | --- |
| `index` | entier | la position, négative ou non |
| `value` | n'importe quoi | la valeur qui y est stockée |

Un vecteur se sérialise en ses seules entrées, par indice croissant. La valeur par défaut ne fait pas partie de cette forme ; il faut donc la transporter à côté lors d'un aller-retour

## TypeScript

Publié (`@calbona/sparse-vector`). Usage et référence de l'API dans [`typescript/README.md`](../typescript/README.md)

## C++

Une implémentation C++17 entièrement en en-têtes, à prendre depuis le dépôt ; pas encore sur vcpkg ni Conan. Usage et référence de l'API dans [`c++/README.md`](../c++/README.md)

## Rust

Publié sous le nom `sparse-vector-rs`. Usage et référence de l'API dans [`rust/README.md`](../rust/README.md)

## Licence

MIT
