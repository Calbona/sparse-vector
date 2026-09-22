import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector } from '../dist/index.js';

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
