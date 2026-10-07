// Benchmark of the TypeScript implementation:
//
//   node benchmark/typescript/bench.mjs
//
// Prints one line per operation, "<name>\t<median milliseconds>", after warming up and taking the
// median of several rounds. The index sequence, the element count and the timing discipline are
// the same in all three implementations, so the numbers can be read side by side.

import { performance } from 'node:perf_hooks';

import { SV_vector } from '../../typescript/dist/index.js';

const SIZE = 100_000;
const ROUNDS = 21;

/** The pseudo-random index sequence, identical in all three implementations. */
function indexes(count) {
  const out = new Int32Array(count);
  let state = 12345;
  for (let i = 0; i < count; i += 1) {
    state = ((Math.imul(state, 1103515245) + 12345) >>> 0) % 2147483648;
    out[i] = (state % 2000000) - 1000000;
  }
  return out;
}

const INDEXES = indexes(SIZE);
const ELEMENTS = Array.from(INDEXES, (index, i) => ({ index, value: i + 1 }));

function median(samples) {
  const sorted = [...samples].sort((a, b) => a - b);
  return sorted[(sorted.length - 1) >> 1];
}

function time(name, run, prepare = () => undefined) {
  for (let i = 0; i < 3; i += 1) {
    run(prepare());
  }
  const samples = [];
  for (let i = 0; i < ROUNDS; i += 1) {
    const input = prepare();
    const start = performance.now();
    run(input);
    samples.push(performance.now() - start);
  }
  console.log(`${name}\t${median(samples).toFixed(3)}`);
}

function filled() {
  const vector = new SV_vector(0);
  for (let i = 0; i < SIZE; i += 1) {
    vector.set(INDEXES[i], i + 1);
  }
  return vector;
}

function sevens() {
  const vector = new SV_vector(0);
  for (let i = 0; i < SIZE; i += 1) {
    vector.set(INDEXES[i], 7);
  }
  return vector;
}

console.log(`stored\t${filled().getElementAmount}`);

time('set', () => {
  const vector = new SV_vector(0);
  for (let i = 0; i < SIZE; i += 1) {
    vector.set(INDEXES[i], i + 1);
  }
});

time(
  'get',
  (vector) => {
    let sum = 0;
    for (let i = 0; i < SIZE; i += 1) {
      sum += vector.get(INDEXES[i]);
    }
    return sum;
  },
  filled,
);

time('fromElements', () => SV_vector.fromElements(ELEMENTS, 0));

time(
  'iterate',
  (vector) => {
    let sum = 0;
    for (const element of vector) {
      sum += element.value;
    }
    return sum;
  },
  filled,
);

time(
  'replace default (full prune)',
  (vector) => {
    vector.setDefaultValue = 7;
  },
  sevens,
);

time(
  'replace predicate (full prune)',
  (vector) => {
    vector.setEquality = () => true;
  },
  filled,
);

// The pair is built once and reused: rebuilding it every round would leave the heap churned right
// before the timed region, and both operations are pure reads.
const EQUAL_PAIR = [filled(), filled()];
const DIFFERING_PAIR = [filled(), filled()];
DIFFERING_PAIR[1].set(INDEXES[SIZE - 1], 999999999);

time('isEqualTo (equal)', ([a, b]) => a.isEqualTo(b), () => EQUAL_PAIR);

time('differences (one element)', ([a, b]) => a.differences(b), () => DIFFERING_PAIR);
