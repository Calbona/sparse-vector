import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector } from '../dist/index.js';

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
