import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector } from '../dist/index.js';

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
