import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector } from '../dist/index.js';

describe('pruning of default-valued elements', () => {
  it('never stores a value equal to the default', () => {
    const vector = new SV_vector();
    vector.set(1, 0);
    assert.equal(vector.getElementAmount, 0);
    assert.deepEqual(vector.elements(), []);
  });

  it('prunes an element that becomes the default', () => {
    const vector = new SV_vector(0);
    vector.set(1, 5);
    assert.equal(vector.getElementAmount, 1);

    vector.set(1, 0);
    assert.equal(vector.getElementAmount, 0);
    assert.equal(vector.get(1), 0);
  });

  it('prunes on a change of default and forgets pruned positions', () => {
    const vector = new SV_vector<number | null>(0);
    vector.set(1, null);
    vector.set(2, 7);
    assert.deepEqual(vector.elements(), [
      { index: 2, value: 7 },
      { index: 1, value: null },
    ]);

    vector.setDefaultValue = null;
    // Position 1 was explicitly null and is dropped; position 2 keeps its value.
    assert.deepEqual(vector.elements(), [{ index: 2, value: 7 }]);
    // The old default of 0 is not remembered: position 3 reads back as null.
    assert.equal(vector.get(3), null);
    assert.equal(vector.getElementAmount, 1);
  });

  it('keeps elements that were pruned earlier gone', () => {
    const vector = new SV_vector<number | null>(0);
    vector.set(1, 0);
    vector.set(2, 7);
    assert.deepEqual(vector.indexes(), [2]);

    vector.setDefaultValue = null;
    vector.setDefaultValue = 0;
    assert.deepEqual(vector.elements(), [{ index: 2, value: 7 }]);
  });

  it('compares strictly', () => {
    const vector = new SV_vector<unknown>(0);
    // 0, '0' and false are all falsy but only 0 is the default.
    vector.set(1, '0');
    vector.set(2, false);
    vector.set(3, null);
    assert.equal(vector.getElementAmount, 3);
  });

  it('documents the NaN caveat', () => {
    const vector = new SV_vector<number>(NaN);
    vector.set(1, NaN);
    // NaN !== NaN, so strict pruning cannot recognise it as the default.
    assert.equal(vector.getElementAmount, 1);
  });
});
