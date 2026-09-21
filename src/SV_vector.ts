import type { SV_element } from './SV_element';

/**
 * a sparse vector: a mapping from integer index to an arbitrary value, in which
 * only the positions that differ from a default value are stored
 *
 * Because only non-default positions are kept, an SV_vector occupies O(k) memory for k stored entries no matter how far apart the indices are, and whether they are large or negative. Indices need not be contiguous, and an index holds no explicit entry simply reports the {@link defaultValue}.
 */
export class SV_vector<T = number> {
  private readonly _entries = new Map<number, T>();
  private _default: T;

  /**
   * @param defaultValue value reported for every position that has no explicit entry, defaults to the number `0`
   */
  constructor(...args: [] | [defaultValue: T]) {
    this._default = args.length > 0 ? (args[0] as T) : (0 as unknown as T);
  }

  get defaultValue(): T {
    return this._default;
  }

  set defaultValue(next: T) {
    this._default = next;
    this._prune();
  }

  /**
   * number of explicitly stored entries
   */
  get size(): number {
    return this._entries.size;
  }

  /** whether `index` has an explicitly stored entry */
  has(index: number): boolean {
    return this._entries.has(assertIndex(index));
  }

  /**
   * get `value` at `index`
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  get(index: number): T {
    assertIndex(index);
    return this._entries.has(index) ? (this._entries.get(index) as T) : this._default;
  }

  /**
   * insert or update `value` at `index`
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  set(index: number, value: T): this {
    assertIndex(index);
    if (value === this._default) {
      this._entries.delete(index);
    } else {
      this._entries.set(index, value);
    }
    return this;
  }

  /**
   * remove the explicit entry at `index`, so it reads back as {@link defaultValue}
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  delete(index: number): boolean {
    return this._entries.delete(assertIndex(index));
  }

  /**
   * remove every explicit entry
   * default value kept
   */
  clear(): void {
    this._entries.clear();
  }

  /** get explicit entries in ascending index order */
  elements(): SV_element<T>[] {
    return this._sortedIndices().map((index) => ({ index, value: this._stored(index) }));
  }

  /** get indices holding an explicit entry in ascending order */
  keys(): number[] {
    return this._sortedIndices();
  }

  /** get stored values in ascending order */
  values(): T[] {
    return this._sortedIndices().map((index) => this._stored(index));
  }

  /** iterates the explicit entries in ascending order */
  [Symbol.iterator](): IterableIterator<SV_element<T>> {
    return this.elements()[Symbol.iterator]();
  }

  /** JSON form of this vector */
  toJSON(): SV_element<T>[] {
    return this.elements();
  }

  /** an independent copy, default value included */
  clone(): SV_vector<T> {
    const copy = new SV_vector<T>(this._default);
    for (const [index, value] of this._entries) {
      copy._entries.set(index, value);
    }
    return copy;
  }

  /**
   * builds a vector from a sequence of elements
   * Entries strictly equal to the default are dropped
   * For a repeated index, the last element wins.
   *
   * @param elements entries to store, in any order
   * @param defaultValue same meaning as in the constructor
   * @throws {TypeError} If any element has a non-integer index.
   */
  static from<T>(
    elements: Iterable<SV_element<T>>,
    ...args: [] | [defaultValue: T]
  ): SV_vector<T> {
    const vector = new SV_vector<T>(...args);
    for (const { index, value } of elements) {
      vector.set(index, value);
    }
    return vector;
  }

  /** the indices of every stored entry */
  private _sortedIndices(): number[] {
    return [...this._entries.keys()].sort((a, b) => a - b);
  }

  /**the value stored at an index already known to be present */
  private _stored(index: number): T {
    return this._entries.get(index) as T;
  }

  /**
   * drops every entry strictly equal to the current default
   */
  private _prune(): void {
    for (const [index, value] of this._entries) {
      if (value === this._default) {
        this._entries.delete(index);
      }
    }
  }
}

/**
 * SV_vector indices are integers, negative ones included
 */
function assertIndex(index: number): number {
  if (!Number.isInteger(index)) {
    throw new TypeError(`SV_vector index must be an integer, received ${String(index)}`);
  }
  return index;
}
