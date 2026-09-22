import assert from 'node:assert/strict';
import { describe, it } from 'node:test';

// Tests run against the built artifact, which is what consumers actually get.
import { SV_vector, type SV_element } from '../dist/index.js';

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
