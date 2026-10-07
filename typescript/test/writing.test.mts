import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector } from '../dist/index.js';

describe('writing', () => {
  it('inserts and updates', () => {
    const vector = new SV_vector<string>('');
    vector.set(1, 'a');
    assert.equal(vector.get(1), 'a');
    assert.equal(vector.getElementAmount, 1);

    vector.set(1, 'b');
    assert.equal(vector.get(1), 'b');
    assert.equal(vector.getElementAmount, 1);
  });

  it('resets one element to the default value', () => {
    const vector = new SV_vector<string>('');
    vector.set(1, 'a');
    assert.equal(vector.resetValue(1), true);
    assert.equal(vector.get(1), '');
    assert.equal(vector.getElementAmount, 0);
    assert.equal(vector.resetValue(1), false);
  });

  it('resets every element but keeps the default', () => {
    const vector = new SV_vector<string>('gone');
    assert.equal(vector.resetVector(), false);
    vector.set(1, 'a');
    vector.set(-2, 'b');
    assert.equal(vector.resetVector(), true);
    assert.equal(vector.resetVector(), false);
    assert.equal(vector.getElementAmount, 0);
    assert.equal(vector.getDefaultValue, 'gone');
    assert.equal(vector.get(1), 'gone');
  });
});
