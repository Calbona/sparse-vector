import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector, type SV_element } from '../dist/index.js';

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

  it('reaches an entry by ordinal', () => {
    const vector = new SV_vector<string>('');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    vector.set(5, 'e');

    // Ordinals number the entries, not the positions: 0 is the leftmost stored
    // entry however far out its index lies.
    assert.deepEqual(vector.element(0), { index: -2, value: 'b' });
    assert.deepEqual(vector.element(2), { index: 5, value: 'e' });
    assert.equal(vector.elementIndex(1), 0);
    assert.equal(vector.elementValue(1), 'c');
  });

  it('counts from the right by ordinal', () => {
    const vector = new SV_vector<string>('');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    vector.set(5, 'e');

    assert.deepEqual(vector.lastElement(0), { index: 5, value: 'e' });
    assert.deepEqual(vector.lastElement(2), { index: -2, value: 'b' });
    assert.equal(vector.lastElementIndex(1), 0);
    assert.equal(vector.lastElementValue(1), 'c');
  });

  it('refuses an ordinal without a matching entry', () => {
    const vector = new SV_vector<string>('');
    vector.set(1, 'a');

    // One entry stored, so ordinal 0 reaches it and ordinal 1 is already past
    // the end, wherever that entry's index happens to lie.
    assert.throws(() => vector.element(1), RangeError);
    assert.throws(() => vector.elementIndex(1), RangeError);
    assert.throws(() => vector.elementValue(1), RangeError);
    assert.throws(() => vector.lastElement(1), RangeError);
    assert.throws(() => vector.lastElementIndex(1), RangeError);
    assert.throws(() => vector.lastElementValue(1), RangeError);

    // Ordinals start at 0, so a negative one is out of range rather than
    // counting back from the other end.
    assert.throws(() => vector.element(-1), RangeError);
    assert.throws(() => vector.lastElement(-1), RangeError);
    assert.throws(() => vector.element(0.5), TypeError);

    const empty = new SV_vector<string>('');
    assert.throws(() => empty.element(0), RangeError);
    assert.throws(() => empty.lastElement(0), RangeError);
  });

  it('measures significant positions from the first entry', () => {
    const vector = new SV_vector<string>('');
    vector.set(10, 'a');
    vector.set(13, 'd');

    assert.equal(vector.leftSignificantValue(0), 'a');
    // Positions holding no entry count, the way the zeros inside a number count.
    assert.equal(vector.leftSignificantValue(2), '');
    assert.equal(vector.leftSignificantValue(3), 'd');
    // A negative offset walks off the left of that first entry, into the default.
    assert.equal(vector.leftSignificantValue(-1), '');
  });

  it('measures significant positions from the last entry', () => {
    const vector = new SV_vector<string>('');
    vector.set(10, 'a');
    vector.set(13, 'd');

    assert.equal(vector.rightSignificantValue(0), 'd');
    assert.equal(vector.rightSignificantValue(3), 'a');
    assert.equal(vector.rightSignificantValue(4), '');
    assert.equal(vector.rightSignificantValue(-1), '');
  });

  it('refuses a significant position without an anchor', () => {
    const empty = new SV_vector<string>('');
    assert.throws(() => empty.leftSignificantValue(0), RangeError);
    assert.throws(() => empty.rightSignificantValue(0), RangeError);
  });

  // No C++ or Rust counterpart: their index ranges are integers that overflow.
  it('handles a position at the edge of the index range', () => {
    const vector = new SV_vector(0);
    vector.set(Number.MAX_SAFE_INTEGER, 1);

    // The only entry is the anchor. Stepping right of it leaves the safe integer
    // range, where positions can no longer be stored, so the default comes back.
    assert.equal(vector.leftSignificantValue(0), 1);
    assert.equal(vector.leftSignificantValue(1), 0);
    assert.equal(vector.leftSignificantValue(-Number.MAX_SAFE_INTEGER), 0);
  });
});
