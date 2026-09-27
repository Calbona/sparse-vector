import type { SV_element } from './SV_element';

/**
 * a sparse vector: a mapping from integer index to an arbitrary value that stores only the
 * positions differing from the default value, largest index leftmost
 */
export class SV_vector<T = number> {
  private readonly _entries = new Map<number, T>();
  private _default: T;
  private _sortedIndexes: number[] | undefined;

  /** @param defaultValue read back at every position with no explicit entry; `0` when omitted */
  constructor(...args: [] | [defaultValue: T]) {
    this._default = args.length > 0 ? (args[0] as T) : (0 as unknown as T);
  }

  get getDefaultValue(): T {
    return this._default;
  }

  set setDefaultValue(next: T) {
    this._default = next;
    this._prune();
  }

  /** number of explicitly stored entries */
  get getElementAmount(): number {
    return this._entries.size;
  }

  /** distance between the leftmost and rightmost entries, both included; 0 when nothing is stored */
  get getSignificantDimension(): number {
    const ascending = this._ascendingIndexes();
    const rightmost = ascending[0];
    const leftmost = ascending[ascending.length - 1];
    return leftmost === undefined || rightmost === undefined ? 0 : leftmost - rightmost + 1;
  }

  /** the leftmost index when positive, else 0 */
  get getPlusDimension(): number {
    const ascending = this._ascendingIndexes();
    const leftmost = ascending[ascending.length - 1] ?? 0;
    return leftmost > 0 ? leftmost : 0;
  }

  /** the rightmost index negated when negative, else 0 */
  get getMinusDimension(): number {
    const rightmost = this._ascendingIndexes()[0] ?? 0;
    return rightmost < 0 ? -rightmost : 0;
  }

  /**
   * the value at `index`, or the default when nothing is stored there
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  get(index: number): T {
    assertIndex(index);
    return this._entries.has(index) ? (this._entries.get(index) as T) : this._default;
  }

  /**
   * inserts or updates the entry at `index`, returning `this`
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  set(index: number, value: T): this {
    assertIndex(index);
    if (value === this._default) {
      if (this._entries.delete(index)) {
        this._invalidate();
      }
    } else {
      if (!this._entries.has(index)) {
        this._invalidate();
      }
      this._entries.set(index, value);
    }
    return this;
  }

  /**
   * resets the entry at `index` to the default, returning whether there was one
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  resetValue(index: number): boolean {
    const removed = this._entries.delete(assertIndex(index));
    if (removed) {
      this._invalidate();
    }
    return removed;
  }

  /** resets every entry, keeping the default */
  resetVector(): void {
    if (this._entries.size > 0) {
      this._entries.clear();
      this._invalidate();
    }
  }

  /** every entry, descending by index */
  elements(): SV_element<T>[] {
    return this._orderedIndices().map((index) => ({ index, value: this._stored(index) }));
  }

  /** every entry, ascending by index */
  invertedElements(): SV_element<T>[] {
    return this._orderedIndices(true).map((index) => ({ index, value: this._stored(index) }));
  }

  /** every stored index, descending */
  indexes(): number[] {
    return this._orderedIndices();
  }

  /** every stored index, ascending */
  invertedIndexes(): number[] {
    return this._orderedIndices(true);
  }

  /** every stored value, descending by index */
  values(): T[] {
    return this._orderedIndices().map((index) => this._stored(index));
  }

  /** every stored value, ascending by index */
  invertedValues(): T[] {
    return this._orderedIndices(true).map((index) => this._stored(index));
  }

  /**
   * the (n+1)-th entry from the left, the ordinal numbering the entries and not the positions
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  element(n: number): SV_element<T> {
    const index = this.elementIndex(n);
    return { index, value: this._stored(index) };
  }

  /**
   * the index {@link element} sits at
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  elementIndex(n: number): number {
    return this._ordinalIndex(n, false);
  }

  /**
   * the value {@link element} holds
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  elementValue(n: number): T {
    return this._stored(this.elementIndex(n));
  }

  /**
   * the (n+1)-th entry from the right, `invertedElement(0)` being the rightmost
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  invertedElement(n: number): SV_element<T> {
    const index = this.invertedElementIndex(n);
    return { index, value: this._stored(index) };
  }

  /**
   * the index {@link invertedElement} sits at
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  invertedElementIndex(n: number): number {
    return this._ordinalIndex(n, true);
  }

  /**
   * the value {@link invertedElement} holds
   *
   * @throws {RangeError} If `n` is negative, or no explicit entry sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  invertedElementValue(n: number): T {
    return this._stored(this.invertedElementIndex(n));
  }

  /**
   * the value `n` positions right of the leftmost entry, empty positions counting; a negative `n`
   * walks left of it, into the default value
   *
   * @throws {RangeError} If no entry is stored at all, so there is nothing to measure from.
   * @throws {TypeError} If `n` is not an integer.
   */
  leftSignificantValue(n: number): T {
    assertIndex(n);
    const first = this._anchor(false);
    return this.get(first - n);
  }

  /**
   * the value `n` positions left of the rightmost entry, a negative `n` walking right of it
   *
   * @throws {RangeError} If no entry is stored at all, so there is nothing to measure from.
   * @throws {TypeError} If `n` is not an integer.
   */
  rightSignificantValue(n: number): T {
    assertIndex(n);
    const last = this._anchor(true);
    return this.get(last + n);
  }

  /** every entry, descending by index, read as the walk goes */
  *[Symbol.iterator](): IterableIterator<SV_element<T>> {
    const ascending = this._ascendingIndexes();
    for (let i = ascending.length - 1; i >= 0; i -= 1) {
      const index = ascending[i] as number;
      yield { index, value: this._stored(index) };
    }
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
   * builds from any iterable of elements, dropping those equal to the default and keeping the
   * last of a repeated index
   *
   * @param elements entries to store, in any order
   * @param defaultValue same meaning as in the constructor
   * @throws {TypeError} If any element has a non-integer index.
   */
  static fromElements<T>(
    elements: Iterable<SV_element<T>>,
    ...args: [] | [defaultValue: T]
  ): SV_vector<T> {
    const vector = new SV_vector<T>(...args);
    for (const { index, value } of elements) {
      vector.set(index, value);
    }
    return vector;
  }

  /** the indices of every stored entry, descending, or ascending when `inverted` */
  private _orderedIndices(inverted = false): number[] {
    const ascending = this._ascendingIndexes();
    return inverted ? ascending.slice() : ascending.slice().reverse();
  }

  /** every stored index, ascending, sorted once and kept until the entries change */
  private _ascendingIndexes(): number[] {
    let sorted = this._sortedIndexes;
    if (sorted === undefined) {
      sorted = [...this._entries.keys()].sort((a, b) => a - b);
      this._sortedIndexes = sorted;
    }
    return sorted;
  }

  private _invalidate(): void {
    this._sortedIndexes = undefined;
  }

  private _ordinalIndex(n: number, inverted: boolean): number {
    assertOrdinal(n);
    const ascending = this._ascendingIndexes();
    const index = inverted ? ascending[n] : ascending[ascending.length - 1 - n];
    if (index === undefined) {
      const side = inverted ? 'right' : 'left';
      throw new RangeError(
        `SV_vector holds ${ascending.length} entries, so there is no entry ${n} from the ${side}`,
      );
    }
    return index;
  }

  private _anchor(inverted: boolean): number {
    const ascending = this._ascendingIndexes();
    const index = inverted ? ascending[0] : ascending[ascending.length - 1];
    if (index === undefined) {
      throw new RangeError('SV_vector holds no entries, so there is nothing to measure from');
    }
    return index;
  }

  private _stored(index: number): T {
    return this._entries.get(index) as T;
  }

  private _prune(): void {
    let pruned = false;
    for (const [index, value] of this._entries) {
      if (value === this._default) {
        this._entries.delete(index);
        pruned = true;
      }
    }
    if (pruned) {
      this._invalidate();
    }
  }
}

/** SV_vector indices are integers, negative ones included */
function assertIndex(index: number): number {
  return assertInteger(index, 'index');
}

/** SV_vector ordinals number the entries, starting at 0 and never negative */
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
