[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · [Español](es-ES.md) · **Français** · [Русский](ru-RU.md)

# sparse-vector

Un vecteur creux pour TypeScript : une correspondance d'indices entiers vers des valeurs quelconques, qui ne stocke que les positions différentes d'une valeur par défaut

Les indices peuvent être négatifs et n'ont pas besoin d'être contigus

Ainsi, un vecteur qui ne contient que trois entrées n'occupe que trois entrées — que ces trois indices soient `0, 1, 2` ou `-10^9, 0, 10^9`

Cette bibliothèque n'est délibérément **pas** un vecteur au sens mathématique

## Installation

```sh
npm install sparse-vector
```

## Utilisation

```ts
import { SV_vector } from 'sparse-vector';

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

## API

### `new SV_vector<T>(defaultValue?)`

Crée un vecteur

`defaultValue` est la valeur par défaut des positions vides, c'est-à-dire la valeur rapportée pour toute position sans entrée explicite ; par défaut, le `0` de type number

`T` vaut `number` par défaut

### Propriétés

| Membre | Description |
| --- | --- |
| `defaultValue: T` | Lisible et modifiable ; l'affectation écarte immédiatement toutes les entrées strictement égales à la nouvelle valeur par défaut |
| `size: number` | Nombre d'entrées stockées explicitement |

### Méthodes

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

### `SV_vector.from(elements, defaultValue?)`

Construit à partir d'un itérable de `SV_element` ; les entrées strictement égales à la valeur par défaut sont écartées ; pour un indice répété, la dernière l'emporte

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

C'est le format de sérialisation : un objet JSON ordinaire avec exactement les deux clés `index` et `value`

À noter : `toJSON()` ne produit que les entrées, la valeur par défaut ne fait pas partie de cette structure ; il faut donc la transporter lors d'un aller-retour JSON

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

## Règles d'écartement

Une entrée égale à la valeur par défaut des positions vides n'est jamais conservée

L'écartement se fait par **égalité stricte**, d'où deux conséquences :

- `0`, `'0'`, `false` et `null` sont quatre valeurs distinctes ; seule une correspondance `===` est écartée
- `NaN` n'est pas strictement égal à `NaN`, donc un `NaN` stocké est conservé même si la valeur par défaut est elle-même `NaN`

## Développement

```sh
npm run build      # compile vers dist/
npm run typecheck  # vérifie les types de src et des tests
npm test           # compile d'abord, puis teste
```

Nécessite Node 24+

La bibliothèque elle-même n'a aucune dépendance à l'exécution

## License

MIT
