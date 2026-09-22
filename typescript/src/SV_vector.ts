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

  /**
   * get the element at position `n` of {@link elements()}, i.e. the (n+1)-th explicit entry counting from the left
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  element(n: number): SV_element<T> {
    const index = this.elementIndex(n);
    return { index, value: this._stored(index) };
  }

  /**
   * get the index of the element at position `n` of {@link elements()}
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  elementIndex(n: number): number {
    return this._ordinalIndex(n, false);
  }

  /**
   * get the value of the element at position `n` of {@link elements()}
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  elementValue(n: number): T {
    return this._stored(this.elementIndex(n));
  }

  /**
   * get the element at position `n` of {@link elements()} counted from the right, i.e. the last explicit entry for `n` of 0
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  lastElement(n: number): SV_element<T> {
    const index = this.lastElementIndex(n);
    return { index, value: this._stored(index) };
  }

  /**
   * get the index of the element at position `n` of {@link elements()} counted from the right
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  lastElementIndex(n: number): number {
    return this._ordinalIndex(n, true);
  }

  /**
   * get the value of the element at position `n` of {@link elements()} counted from the right
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  lastElementValue(n: number): T {
    return this._stored(this.lastElementIndex(n));
  }

  /**
   * Treating the first explicit entry as the first significant digit, get the
   * value `n` positions to its right. Empty positions in between count, the way
   * the zeros inside a number count towards its sign
   *
   * `n` may be negative, which walks left of that first entry instead
   *
   * @throws {RangeError} If no entry is stored at all, so there is nothing to measure from.
   * @throws {TypeError} If `n` is not an integer.
   */
  leftSignificantValue(n: number): T {
    assertIndex(n);
    const first = this._anchor(true);
    return this.get(first + n);
  }

  /**
   * Treating the last explicit entry as the first significant digit counting
   * from the right, get the value `n` positions to its left. Empty positions in
   * between count, the way the zeros inside a number count towards its sign
   *
   * `n` may be negative, which walks right of that last entry instead
   *
   * @throws {RangeError} If no entry is stored at all, so there is nothing to measure from.
   * @throws {TypeError} If `n` is not an integer.
   */
  rightSignificantValue(n: number): T {
    assertIndex(n);
    const last = this._anchor(false);
    return this.get(last - n);
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

  /** the index of the entry at 0-based ordinal `n`, from the left or from the right */
  private _ordinalIndex(n: number, fromRight: boolean): number {
    assertOrdinal(n);
    const indices = this._sortedIndices();
    const index = fromRight ? indices[indices.length - 1 - n] : indices[n];
    if (index === undefined) {
      const side = fromRight ? 'right' : 'left';
      throw new RangeError(
        `SV_vector holds ${indices.length} entries, so there is no entry ${n} from the ${side}`,
      );
    }
    return index;
  }

  /** the outermost stored index, the first one or the last one */
  private _anchor(fromLeft: boolean): number {
    const indices = this._sortedIndices();
    const index = fromLeft ? indices[0] : indices[indices.length - 1];
    if (index === undefined) {
      throw new RangeError('SV_vector holds no entries, so there is nothing to measure from');
    }
    return index;
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
  return assertInteger(index, 'index');
}

/**
 * SV_vector ordinals number the entries from one end, so they start at 0 and are
 * never negative
 */
function assertOrdinal(n: number): number {
  const ordinal = assertInteger(n, 'ordinal');
  if (ordinal < 0) {
    throw new RangeError(`SV_vector ordinal must not be negative, received ${String(ordinal)}`);
  }
  return ordinal;
}

function assertInteger(value: number, noun: string): number {
  if (!Number.isInteger(value)) {
    throw new TypeError(`SV_vector ${noun} must be an integer, received ${String(value)}`);
  }
  return value;
}
