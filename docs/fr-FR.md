[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · [Español](es-ES.md) · **Français** · [Русский](ru-RU.md)

# sparse-vector

Un vecteur creux : une correspondance d'indices entiers vers des valeurs quelconques, qui ne stocke que les positions différentes d'une valeur par défaut

## Concept

Les indices peuvent être négatifs et n'ont pas besoin d'être contigus

Ainsi, un vecteur qui ne contient que trois entrées n'occupe que trois entrées — que ces trois indices soient `0, 1, 2` ou `-10^9, 0, 10^9`

Cette bibliothèque n'est délibérément **pas** un vecteur au sens mathématique. Elle ne porte aucune arithmétique ; c'est une structure de données, et rien d'autre

## Implémentations

Le même type est proposé dans trois langages. Ils partagent toutes les règles ci-dessous — seule l'écriture diffère

| Langage | Paquet | Répertoire | État |
| --- | --- | --- | --- |
| TypeScript | `@calbona/sparse-vector` | [`typescript/`](../typescript/) | publié |
| C++ | — | [`c++/`](../c++/) | prévu |
| Rust | `sparse-vector-rs` | [`rust/`](../rust/) | prévu |

Chaque répertoire d'implémentation possède son propre README, avec l'installation, l'utilisation et la référence d'API de ce langage. Cette page définit la sémantique qu'ils ont en commun, pour ne pas avoir à la répéter trois fois

## Sémantique commune à toutes les implémentations

### La valeur par défaut

Un vecteur est créé avec une valeur par défaut : la valeur rapportée pour toute position qui ne porte aucune entrée explicite. Elle vaut par défaut le `0` de type number, sauf si une autre est fournie

La valeur par défaut peut être remplacée plus tard. La remplacer écarte immédiatement toutes les entrées qui sont égales à la nouvelle valeur par défaut

Comme chaque position a une valeur définie, la lecture est totale. Tout entier — stocké ou non, dans les limites ou bien au-delà — renvoie une valeur plutôt que de lever une erreur

### Une entrée égale à la valeur par défaut n'est jamais conservée

Écrire la valeur par défaut dans une position revient à supprimer ce qui s'y trouvait. C'est ce qui garde la structure creuse : la mémoire est en O(k) selon le nombre d'entrées qui diffèrent réellement de la valeur par défaut, quelle que soit la distance entre les indices, et même s'ils sont très négatifs

### L'écartement se fait par égalité stricte

Chaque langage écarte avec sa propre égalité la plus stricte — `===` en TypeScript, `==` en C++, `PartialEq` en Rust. D'où deux conséquences :

- `0`, `'0'`, `false` et `null` sont quatre valeurs distinctes ; seule une correspondance exacte écarte une entrée
- `NaN` n'est jamais égal à lui-même, donc un `NaN` stocké est conservé même si la valeur par défaut est elle-même `NaN`

### Sérialisation

Une entrée est un objet ordinaire avec exactement deux clés :

| Clé | Type | Signification |
| --- | --- | --- |
| `index` | entier | la position, négative ou non |
| `value` | n'importe quoi | la valeur qui y est stockée |

Un vecteur se sérialise en ses seules entrées, par indice croissant. La valeur par défaut ne fait pas partie de cette forme ; il faut donc la transporter à côté lors d'un aller-retour

## TypeScript

### Installation

```sh
npm install @calbona/sparse-vector
```

### Utilisation

```ts
import { SV_vector } from '@calbona/sparse-vector';

const vector = new SV_vector(); // valeur par défaut des positions vides : le 0 de type number

vector.set(1_000_000, 'très loin');
vector.set(-42, 'négatif');

vector.get(1_000_000); // 'très loin'
vector.get(-42);       // 'négatif'
vector.get(7);         // 0
vector.size;           // 2
```

La valeur par défaut peut être fournie, puis modifiée plus tard ; toute position sans entrée explicite se lit comme elle

```ts
const counts = new SV_vector<number | null>(null); // les positions vides valent null
counts.set(3, 1);
counts.get(4); // null

counts.defaultValue = 0; // les positions vides valent désormais 0
```

Le type des valeurs n'est pas restreint : tout est permis

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

### API

#### `new SV_vector<T>(defaultValue?)`

Crée un vecteur

`defaultValue` est la valeur par défaut des positions vides, c'est-à-dire la valeur rapportée pour toute position sans entrée explicite ; par défaut, le `0` de type number

`T` vaut `number` par défaut

#### Propriétés

| Membre | Description |
| --- | --- |
| `defaultValue: T` | Lisible et modifiable ; l'affectation écarte immédiatement toutes les entrées strictement égales à la nouvelle valeur par défaut |
| `size: number` | Nombre d'entrées stockées explicitement |

#### Méthodes

| Méthode | Description |
| --- | --- |
| `get(index): T` | La valeur en `index` ; renvoie `defaultValue` quand il n'y a pas d'entrée explicite à cet endroit |
| `set(index, value): this` | Insère ou modifie ; chaînable |
| `has(index): boolean` | Indique s'il existe une entrée explicite en `index` |
| `delete(index): boolean` | Supprime l'entrée explicite à cet endroit et revient à la valeur par défaut |
| `clear(): void` | Supprime toutes les entrées explicites et revient à la valeur par défaut |
| `elements(): SV_element<T>[]` | Les entrées explicites, par indice croissant |
| `keys(): number[]` | Les indices stockés, par ordre croissant |
| `values(): T[]` | Les valeurs stockées, par indice croissant |
| `clone(): SV_vector<T>` | Une copie indépendante |
| `[Symbol.iterator]()` | Itère les entrées explicites par indice croissant |
| `toJSON(): SV_element<T>[]` | Identique à `elements()`, donc `JSON.stringify` fonctionne directement |

`index` doit être un entier — positif ou négatif ; un non-entier lève une `TypeError`

Lire une position hors des données ne lève pas d'erreur, mais renvoie la valeur par défaut : c'est justement tout l'intérêt de ce type

#### `SV_vector.from(elements, defaultValue?)`

Construit à partir d'un itérable de `SV_element` ; les entrées strictement égales à la valeur par défaut sont écartées ; pour un indice répété, la dernière l'emporte

#### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

La forme de sérialisation décrite plus haut. L'aller-retour s'écrit ainsi :

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

### Développement

```sh
npm run build      # compile vers dist/
npm run typecheck  # vérifie les types de src et des tests
npm test           # compile d'abord, puis teste
```

Nécessite Node 24+

La bibliothèque elle-même n'a aucune dépendance à l'exécution

## C++

L'implémentation C++ n'est pas encore publiée. Les types prévus sont `SV_vector` et `SV_element`, avec la même sémantique que les autres langages ci-dessus

## Rust

L'implémentation Rust n'est pas encore publiée. Les types `SparseVector` et `Element` sont prévus dans le crate `sparse-vector-rs`, avec la même sémantique que les autres langages ci-dessus

## Licence

MIT
