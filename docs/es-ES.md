[English](../README.md) · [简体中文](zh-CN.md) · [繁體中文](zh-TW.md) · [日本語](ja-JP.md) · **Español** · [Français](fr-FR.md) · [Русский](ru-RU.md)

# sparse-vector

Un vector disperso para TypeScript: una correspondencia de índices enteros a valores arbitrarios que solo almacena las posiciones distintas de un valor por defecto

Los índices pueden ser negativos y no hace falta que sean contiguos

Así, un vector con solo tres entradas ocupa solo tres entradas — tanto si esos tres índices son `0, 1, 2` como si son `-10^9, 0, 10^9`

Esta biblioteca deliberadamente **no** es un vector en el sentido matemático

## Instalación

```sh
npm install sparse-vector
```

## Uso

```ts
import { SV_vector } from 'sparse-vector';

const vector = new SV_vector(); // valor por defecto de las posiciones vacías: el 0 de tipo number

vector.set(1_000_000, 'muy lejos');
vector.set(-42, 'negativo');

vector.get(1_000_000); // 'muy lejos'
vector.get(-42);       // 'negativo'
vector.get(7);         // 0
vector.size;           // 2
```

El valor por defecto se puede indicar y también cambiar después; toda posición sin entrada explícita se lee como él

```ts
const counts = new SV_vector<number | null>(null); // las posiciones vacías son null
counts.set(3, 1);
counts.get(4); // null

counts.defaultValue = 0; // las posiciones vacías pasan a ser 0
```

El tipo de los valores no tiene restricción: vale cualquier cosa

```ts
const tagged = new SV_vector<unknown>();
tagged.set(0, { kind: 'header' });
```

## API

### `new SV_vector<T>(defaultValue?)`

Crea un vector

`defaultValue` es el valor por defecto de las posiciones vacías, es decir, el valor que se informa para toda posición sin entrada explícita; si se omite, es el `0` de tipo number

`T` es `number` por defecto

### Propiedades

| Miembro | Descripción |
| --- | --- |
| `defaultValue: T` | Legible y asignable; al asignarlo se descartan de inmediato todas las entradas estrictamente iguales al nuevo valor por defecto |
| `size: number` | Número de entradas almacenadas explícitamente |

### Métodos

| Método | Descripción |
| --- | --- |
| `get(index): T` | El valor en `index`; devuelve `defaultValue` cuando ahí no hay entrada explícita |
| `set(index, value): this` | Inserta o modifica; se puede encadenar |
| `has(index): boolean` | Si existe una entrada explícita en `index` |
| `delete(index): boolean` | Elimina la entrada explícita de ahí y vuelve al valor por defecto |
| `clear(): void` | Elimina todas las entradas explícitas y vuelve al valor por defecto |
| `elements(): SV_element<T>[]` | Las entradas explícitas, en orden ascendente de índice |
| `keys(): number[]` | Los índices almacenados, en orden ascendente |
| `values(): T[]` | Los valores almacenados, en orden ascendente de índice |
| `clone(): SV_vector<T>` | Una copia independiente |
| `[Symbol.iterator]()` | Itera las entradas explícitas en orden ascendente de índice |
| `toJSON(): SV_element<T>[]` | Igual que `elements()`, así que `JSON.stringify` funciona directamente |

`index` tiene que ser un entero — positivo o negativo; si no es entero lanza `TypeError`

Leer una posición fuera del rango de los datos no lanza error, devuelve el valor por defecto: de eso trata precisamente este tipo

### `SV_vector.from(elements, defaultValue?)`

Construye a partir de un iterable de `SV_element`; las entradas estrictamente iguales al valor por defecto se descartan; si un mismo índice aparece repetido, prevalece el último

### `SV_element<T>`

```ts
interface SV_element<T = number> {
  index: number;
  value: T;
}
```

Este es el formato de serialización: un objeto JSON plano con exactamente las dos claves `index` y `value`

Ojo: `toJSON()` solo emite las entradas, el valor por defecto no forma parte de esta estructura, así que al ir y volver de JSON hay que llevarlo consigo

```ts
const json = JSON.stringify(vector);
const restored = SV_vector.from(JSON.parse(json) as SV_element<T>[], vector.defaultValue);
```

## Reglas de descarte

Una entrada igual al valor por defecto de las posiciones vacías nunca se conserva

El descarte usa **igualdad estricta**, de lo que se siguen dos consecuencias:

- `0`, `'0'`, `false` y `null` son cuatro valores distintos; solo lo que coincide con `===` se descarta
- `NaN` no es estrictamente igual a `NaN`, así que un `NaN` almacenado se conserva aunque el propio valor por defecto sea `NaN`

## Desarrollo

```sh
npm run build      # compila a dist/
npm run typecheck  # comprueba los tipos de src y de las pruebas
npm test           # compila primero y luego prueba
```

Requiere Node 24+

La biblioteca en sí no tiene dependencias en tiempo de ejecución

## License

MIT
