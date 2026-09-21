import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector, type SV_element } from '../dist/index.js';

describe('default value', () => {
  it('defaults to the number 0 when omitted', () => {
    const vector = new SV_vector();
    assert.equal(vector.defaultValue, 0);
    assert.equal(vector.get(5), 0);
  });

  it('accepts an explicit default', () => {
    const vector = new SV_vector<string>('');
    assert.equal(vector.get(5), '');
    assert.equal(vector.get(-5), '');
  });

  it('honours an explicit undefined default', () => {
    const vector = new SV_vector<number | undefined>(undefined);
    assert.equal(vector.defaultValue, undefined);
    assert.equal(vector.get(0), undefined);
  });

  it('is replaceable after construction', () => {
    const vector = new SV_vector(0);
    vector.set(1, 42);
    assert.equal(vector.get(2), 0);

    vector.defaultValue = -1;
    assert.equal(vector.get(2), -1);
    assert.equal(vector.get(1), 42);
  });
});

describe('reading', () => {
  it('answers for any integer, negatives included', () => {
    const vector = new SV_vector();
    vector.set(-3, 3);
    assert.equal(vector.get(-3), 3);
    assert.equal(vector.get(3), 0);
    assert.equal(vector.get(0), 0);
    assert.equal(vector.get(Number.MAX_SAFE_INTEGER), 0);
    assert.equal(vector.get(-Number.MAX_SAFE_INTEGER), 0);
  });

  it('stores values of any type', () => {
    const object = { a: 1 };
    const vector = new SV_vector<unknown>(null);
    vector.set(0, object);
    vector.set(1, false);
    vector.set(2, 'text');
    assert.equal(vector.get(0), object);
    assert.equal(vector.get(1), false);
    assert.equal(vector.get(2), 'text');
  });

  it('rejects a non-integer index', () => {
    const vector = new SV_vector();
    for (const bad of [1.5, NaN, Infinity, -Infinity]) {
      assert.throws(() => vector.get(bad), TypeError);
      assert.throws(() => vector.set(bad, 1), TypeError);
      assert.throws(() => vector.delete(bad), TypeError);
      assert.throws(() => vector.has(bad), TypeError);
    }
  });
});

describe('writing', () => {
  it('inserts and updates', () => {
    const vector = new SV_vector<string>('');
    vector.set(1, 'a');
    assert.equal(vector.get(1), 'a');
    assert.equal(vector.size, 1);

    vector.set(1, 'b');
    assert.equal(vector.get(1), 'b');
    assert.equal(vector.size, 1);
  });

  it('deletes to the default value', () => {
    const vector = new SV_vector<string>('');
    vector.set(1, 'a');
    assert.equal(vector.delete(1), true);
    assert.equal(vector.get(1), '');
    assert.equal(vector.size, 0);
    assert.equal(vector.delete(1), false);
  });

  it('clears every entry but keeps the default', () => {
    const vector = new SV_vector<string>('gone');
    vector.set(1, 'a');
    vector.set(-2, 'b');
    vector.clear();
    assert.equal(vector.size, 0);
    assert.equal(vector.defaultValue, 'gone');
    assert.equal(vector.get(1), 'gone');
  });
});

describe('pruning of default-valued entries', () => {
  it('never stores a value equal to the default', () => {
    const vector = new SV_vector();
    vector.set(1, 0);
    assert.equal(vector.size, 0);
    assert.equal(vector.has(1), false);
  });

  it('prunes an entry that becomes the default', () => {
    const vector = new SV_vector(0);
    vector.set(1, 5);
    assert.equal(vector.size, 1);

    vector.set(1, 0);
    assert.equal(vector.size, 0);
    assert.equal(vector.get(1), 0);
  });

  it('prunes on a change of default and forgets pruned positions', () => {
    const vector = new SV_vector<number | null>(0);
    vector.set(1, null);
    vector.set(2, 7);
    assert.deepEqual(vector.elements(), [
      { index: 1, value: null },
      { index: 2, value: 7 },
    ]);

    vector.defaultValue = null;
    // Position 1 was explicitly null and is dropped; position 2 keeps its value.
    assert.deepEqual(vector.elements(), [{ index: 2, value: 7 }]);
    // The old default of 0 is not remembered: position 3 reads back as null.
    assert.equal(vector.get(3), null);
    assert.equal(vector.size, 1);
  });

  it('keeps entries that were pruned earlier gone', () => {
    const vector = new SV_vector<number | null>(0);
    vector.set(1, 0);
    vector.set(2, 7);
    assert.equal(vector.has(1), false);

    vector.defaultValue = null;
    vector.defaultValue = 0;
    assert.deepEqual(vector.elements(), [{ index: 2, value: 7 }]);
  });

  it('compares strictly', () => {
    const vector = new SV_vector<unknown>(0);
    // 0, '0' and false are all falsy but only 0 is the default.
    vector.set(1, '0');
    vector.set(2, false);
    vector.set(3, null);
    assert.equal(vector.size, 3);
  });

  it('documents the NaN caveat', () => {
    const vector = new SV_vector<number>(NaN);
    vector.set(1, NaN);
    // NaN !== NaN, so strict pruning cannot recognise it as the default.
    assert.equal(vector.size, 1);
  });
});

describe('introspection', () => {
  it('lists elements in ascending index order', () => {
    const vector = new SV_vector<string>('');
    vector.set(5, 'e');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    assert.deepEqual(vector.elements(), [
      { index: -2, value: 'b' },
      { index: 0, value: 'c' },
      { index: 5, value: 'e' },
    ]);
    assert.deepEqual(vector.keys(), [-2, 0, 5]);
    assert.deepEqual(vector.values(), ['b', 'c', 'e']);
  });

  it('is iterable', () => {
    const vector = new SV_vector<string>('');
    vector.set(2, 'b');
    vector.set(1, 'a');
    const seen: SV_element<string>[] = [...vector];
    assert.deepEqual(seen, [
      { index: 1, value: 'a' },
      { index: 2, value: 'b' },
    ]);
  });

  it('serialises to its explicit entries only', () => {
    const vector = new SV_vector<string | number>(0);
    vector.set(1, 'a');
    vector.set(2, 0);
    assert.deepEqual(vector.toJSON(), [{ index: 1, value: 'a' }]);
    assert.equal(JSON.stringify(vector), '[{"index":1,"value":"a"}]');
  });

  it('clones independently', () => {
    const vector = new SV_vector<string | number>(0);
    vector.set(1, 'a');
    const copy = vector.clone();
    copy.set(2, 'b');
    copy.defaultValue = 9;
    assert.deepEqual(vector.elements(), [{ index: 1, value: 'a' }]);
    assert.equal(vector.defaultValue, 0);
    assert.equal(copy.size, 2);
    assert.equal(copy.defaultValue, 9);
  });
});

describe('SV_vector.from', () => {
  it('builds from elements, dropping defaults and favouring the last duplicate', () => {
    const vector = SV_vector.from<string | number>(
      [
        { index: 3, value: 'c' },
        { index: 1, value: 'a' },
        { index: 3, value: 'last' },
        { index: 4, value: 0 },
      ],
      0,
    );
    assert.deepEqual(vector.elements(), [
      { index: 1, value: 'a' },
      { index: 3, value: 'last' },
    ]);
  });

  it('takes a default value', () => {
    const vector = SV_vector.from([{ index: 1, value: 'x' }], 'z');
    assert.equal(vector.defaultValue, 'z');
    assert.equal(vector.get(99), 'z');
  });

  it('round-trips through JSON when the default is carried alongside', () => {
    const original = new SV_vector<string>('none');
    original.set(-1, 'a');
    original.set(10, 'b');

    const json = JSON.stringify(original);
    const restored = SV_vector.from(JSON.parse(json) as SV_element<string>[], 'none');
    assert.deepEqual(restored.elements(), original.elements());
    assert.equal(restored.get(0), 'none');
  });
});
