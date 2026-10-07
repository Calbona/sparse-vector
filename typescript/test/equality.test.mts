import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector } from '../dist/index.js';

describe('equality', () => {
  // Equality by length, standing in for a notion `===` cannot express.
  const byLength = (value: string, defaultValue: string): boolean =>
    value.length === defaultValue.length;

  it('replaces the ordinary comparison', () => {
    const vector = new SV_vector<string>('ab');
    vector.setEquality = byLength;
    vector.set(1, 'xy'); // a different string of the same length
    assert.equal(vector.getElementAmount, 0);
  });

  it('prunes what the new predicate calls equal', () => {
    const vector = new SV_vector<string>('ab');
    vector.set(1, 'xy');
    vector.set(2, 'x');
    assert.equal(vector.getElementAmount, 2);

    vector.setEquality = byLength;
    assert.deepEqual(vector.elements(), [{ index: 2, value: 'x' }]);
  });

  it('reports the predicate', () => {
    const vector = new SV_vector<string>('');
    assert.equal(vector.getEquality, undefined);

    vector.setEquality = byLength;
    assert.equal(vector.getEquality, byLength);
  });

  it('restores the ordinary comparison', () => {
    const vector = new SV_vector<string>('ab');
    vector.setEquality = byLength;
    vector.set(1, 'xy');
    assert.equal(vector.getElementAmount, 0);

    vector.setEquality = undefined;
    vector.set(1, 'xy');
    assert.equal(vector.getElementAmount, 1);
  });

  it('travels with a clone', () => {
    const vector = new SV_vector<string>('ab');
    vector.setEquality = byLength;

    const copy = vector.clone();
    assert.equal(copy.getEquality, byLength);
    copy.set(1, 'xy');
    assert.equal(copy.getElementAmount, 0);
  });

  it('compares the two default values', () => {
    const a = new SV_vector<number>(0);
    assert.equal(a.isEqualTo(new SV_vector<number>(5)), false);
    assert.equal(a.isEqualTo(new SV_vector<number>(0)), true);
  });

  it('compares every element', () => {
    const a = new SV_vector<number>(0);
    const b = new SV_vector<number>(0);
    a.set(1, 7);
    b.set(1, 7);
    assert.equal(a.isEqualTo(b), true);

    b.set(2, 7);
    assert.equal(a.isEqualTo(b), false);
  });

  it('treats a missing position as the default', () => {
    const a = new SV_vector<number>(0);
    const b = new SV_vector<number>(0);
    b.set(3, 9);
    assert.equal(a.isEqualTo(b), false);
    assert.deepEqual(a.differences(b), [{ index: 3, value: 0 }]);
  });

  it('compares across different default values', () => {
    // Not transitive: one is within one of another, and of a third, without the first and third
    // having anything to do with each other.
    const withinOne = (value: number, defaultValue: number): boolean =>
      Math.abs(value - defaultValue) <= 1;

    const a = new SV_vector<number>(0);
    const b = new SV_vector<number>(1);
    a.setEquality = withinOne;
    b.setEquality = withinOne;
    b.set(4, -1); // further than one from b's default, so it stays; within one of a's

    assert.equal(a.getElementAmount, 0);
    assert.equal(b.getElementAmount, 1);
    assert.equal(a.isEqualTo(b), true);
    assert.deepEqual(a.differences(b), []);
  });

  it('lists what differs, descending', () => {
    const a = new SV_vector<number>(0);
    a.set(1, 10);
    a.set(3, 30);
    a.set(5, 50);
    const b = new SV_vector<number>(0);
    b.set(3, 30); // the one they agree on

    assert.deepEqual(a.differences(b), [
      { index: 5, value: 50 },
      { index: 1, value: 10 },
    ]);
    assert.equal(a.isEqualTo(b), false);
  });

  it('carries the default value for a position only the other stores', () => {
    const a = new SV_vector<number>(0);
    const b = new SV_vector<number>(0);
    b.set(2, 7);
    assert.deepEqual(a.differences(b), [{ index: 2, value: 0 }]);
  });

  it('refuses to list differences against an unequal default', () => {
    const a = new SV_vector<number>(0);
    const b = new SV_vector<number>(5);
    assert.throws(() => a.differences(b), TypeError);
  });

  it('documents the NaN caveat for whole-vector comparison', () => {
    const nan = new SV_vector<number>(Number.NaN);
    assert.equal(nan.isEqualTo(nan), false); // NaN is not equal to itself

    const stored = new SV_vector<number>(0);
    stored.set(1, Number.NaN); // kept, since NaN !== NaN
    assert.equal(stored.isEqualTo(stored), false);
    assert.deepEqual(stored.differences(stored), [{ index: 1, value: Number.NaN }]);
  });

  it('refuses to list differences against a NaN default', () => {
    const nan = new SV_vector<number>(Number.NaN);
    assert.throws(() => nan.differences(nan), TypeError);
  });
});
