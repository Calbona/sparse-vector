import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector, type SV_element } from '../dist/index.js';

describe('introspection', () => {
  it('lists elements in descending index order', () => {
    const vector = new SV_vector<string>('');
    vector.set(5, 'e');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    assert.deepEqual(vector.elements(), [
      { index: 5, value: 'e' },
      { index: 0, value: 'c' },
      { index: -2, value: 'b' },
    ]);
    assert.deepEqual(vector.indexes(), [5, 0, -2]);
    assert.deepEqual(vector.values(), ['e', 'c', 'b']);
  });

  it('lists the inverted views in ascending index order', () => {
    const vector = new SV_vector<string>('');
    vector.set(5, 'e');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    assert.deepEqual(vector.invertedElements(), [
      { index: -2, value: 'b' },
      { index: 0, value: 'c' },
      { index: 5, value: 'e' },
    ]);
    assert.deepEqual(vector.invertedIndexes(), [-2, 0, 5]);
    assert.deepEqual(vector.invertedValues(), ['b', 'c', 'e']);

    // The ordinal trio numbers the inverted listing: invertedElement(n) is
    // invertedElements()[n], read from its first entry.
    const listed = vector.invertedElements();
    assert.equal(vector.invertedIndexes()[0], listed[0]?.index);
    assert.deepEqual(vector.invertedElement(1), listed[1]);
  });

  it('reports the entry count', () => {
    const vector = new SV_vector<string>('');
    assert.equal(vector.getElementAmount, 0);

    vector.set(1, 'a');
    assert.equal(vector.getElementAmount, 1);

    vector.set(1, ''); // equal to the default, so dropped again
    assert.equal(vector.getElementAmount, 0);
  });

  it('measures the span between the outermost entries', () => {
    const vector = new SV_vector<string>('');
    // Nothing stored spans nothing.
    assert.equal(vector.getSignificantDimension, 0);

    vector.set(-3, 'a');
    // A single entry spans itself.
    assert.equal(vector.getSignificantDimension, 1);

    // -2 through 5 inclusive: the positions in between count, so this is not
    // the entry count.
    const span = new SV_vector<string>('');
    span.set(5, 'e');
    span.set(-2, 'b');
    assert.equal(span.getSignificantDimension, 8);
  });

  it('measures how far the vector reaches on each side of zero', () => {
    const vector = new SV_vector<string>('');
    // Nothing stored reaches nowhere.
    assert.equal(vector.getPlusDimension, 0);
    assert.equal(vector.getMinusDimension, 0);

    // Entries on one side only leave the other side at zero.
    const positive = new SV_vector<string>('');
    positive.set(3, 'a');
    positive.set(5, 'b');
    assert.equal(positive.getPlusDimension, 5);
    assert.equal(positive.getMinusDimension, 0);

    const negative = new SV_vector<string>('');
    negative.set(-3, 'a');
    negative.set(-5, 'b');
    assert.equal(negative.getPlusDimension, 0);
    assert.equal(negative.getMinusDimension, 5);

    // Straddling zero: 5 above, 2 below, and 8 positions from end to end.
    const span = new SV_vector<string>('');
    span.set(5, 'e');
    span.set(-2, 'b');
    assert.equal(span.getPlusDimension, 5);
    assert.equal(span.getMinusDimension, 2);
    assert.equal(span.getSignificantDimension, 8);
  });

  it('is iterable', () => {
    const vector = new SV_vector<string>('');
    vector.set(2, 'b');
    vector.set(1, 'a');
    const seen: SV_element<string>[] = [...vector];
    assert.deepEqual(seen, [
      { index: 2, value: 'b' },
      { index: 1, value: 'a' },
    ]);
  });

  it('serialises to its explicit entries only', () => {
    const vector = new SV_vector<string | number>(0);
    vector.set(1, 'a');
    vector.set(2, 0);
    // There is no JSON encoder on the class: elements() is the contract, and it
    // is already JSON-safe.
    assert.deepEqual(vector.elements(), [{ index: 1, value: 'a' }]);
    assert.equal(JSON.stringify(vector.elements()), '[{"index":1,"value":"a"}]');
  });

  it('clones independently', () => {
    const vector = new SV_vector<string | number>(0);
    vector.set(1, 'a');
    const copy = vector.clone();
    copy.set(2, 'b');
    copy.setDefaultValue = 9;
    assert.deepEqual(vector.elements(), [{ index: 1, value: 'a' }]);
    assert.equal(vector.getDefaultValue, 0);
    assert.equal(copy.getElementAmount, 2);
    assert.equal(copy.getDefaultValue, 9);
  });

  it('reaches an entry by ordinal', () => {
    const vector = new SV_vector<string>('');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    vector.set(5, 'e');

    // Ordinals number the entries, not the positions: 0 is the leftmost stored
    // entry however far out its index lies.
    assert.deepEqual(vector.element(0), { index: 5, value: 'e' });
    assert.deepEqual(vector.element(2), { index: -2, value: 'b' });
    assert.equal(vector.elementIndex(1), 0);
    assert.equal(vector.elementValue(1), 'c');
  });

  it('counts from the inverted end by ordinal', () => {
    const vector = new SV_vector<string>('');
    vector.set(-2, 'b');
    vector.set(0, 'c');
    vector.set(5, 'e');

    assert.deepEqual(vector.invertedElement(0), { index: -2, value: 'b' });
    assert.deepEqual(vector.invertedElement(2), { index: 5, value: 'e' });
    assert.equal(vector.invertedElementIndex(1), 0);
    assert.equal(vector.invertedElementValue(1), 'c');
  });

  it('refuses an ordinal without a matching entry', () => {
    const vector = new SV_vector<string>('');
    vector.set(1, 'a');

    // One entry stored, so ordinal 0 reaches it and ordinal 1 is past the end.
    assert.throws(() => vector.element(1), RangeError);
    assert.throws(() => vector.elementIndex(1), RangeError);
    assert.throws(() => vector.elementValue(1), RangeError);
    assert.throws(() => vector.invertedElement(1), RangeError);
    assert.throws(() => vector.invertedElementIndex(1), RangeError);
    assert.throws(() => vector.invertedElementValue(1), RangeError);

    // Ordinals start at 0, so a negative one is out of range rather than
    // counting back from the other end.
    assert.throws(() => vector.element(-1), RangeError);
    assert.throws(() => vector.invertedElement(-1), RangeError);
    assert.throws(() => vector.element(0.5), TypeError);

    const empty = new SV_vector<string>('');
    assert.throws(() => empty.element(0), RangeError);
    assert.throws(() => empty.invertedElement(0), RangeError);
  });

  it('measures significant positions from the first entry', () => {
    const vector = new SV_vector<string>('');
    vector.set(10, 'a');
    vector.set(13, 'd');

    // The leftmost entry is the most significant digit.
    assert.equal(vector.leftSignificantValue(0), 'd');
    // Positions holding no entry count, like the zeros inside a number.
    assert.equal(vector.leftSignificantValue(2), '');
    assert.equal(vector.leftSignificantValue(3), 'a');
    // A negative offset walks off the left of that first entry, into the default.
    assert.equal(vector.leftSignificantValue(-1), '');
  });

  it('measures significant positions from the last entry', () => {
    const vector = new SV_vector<string>('');
    vector.set(10, 'a');
    vector.set(13, 'd');

    assert.equal(vector.rightSignificantValue(0), 'a');
    assert.equal(vector.rightSignificantValue(3), 'd');
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

    // The only entry is the anchor, so a step either way lands where nothing is
    // stored. Leftward it also leaves the safe integer range, where positions
    // can no longer be told apart.
    assert.equal(vector.leftSignificantValue(0), 1);
    assert.equal(vector.leftSignificantValue(1), 0);
    assert.equal(vector.leftSignificantValue(-Number.MAX_SAFE_INTEGER), 0);
  });
});
