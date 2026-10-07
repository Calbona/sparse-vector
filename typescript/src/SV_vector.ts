import type { SV_element } from './SV_element';
import type { SV_equality } from './SV_equality';

/**
 * a sparse vector: a mapping from integer index to an arbitrary value that stores only the
 * positions differing from the default value, largest index leftmost
 */
export class SV_vector<T = number> {
  private readonly _elements = new Map<number, T>();
  private _default: T;
  private _equality: SV_equality<T> | undefined;
  private _sortedIndexes: number[] | undefined;

  /** @param defaultValue read back at every position with no explicit element; `0` when omitted */
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

  /** the predicate replacing `===`, or `undefined` when `===` decides */
  get getEquality(): SV_equality<T> | undefined {
    return this._equality;
  }

  /** assigns the predicate, immediately pruning every element it calls equal to the default */
  set setEquality(next: SV_equality<T> | undefined) {
    this._equality = next;
    this._prune();
  }

  /** number of explicitly stored elements */
  get getElementAmount(): number {
    return this._elements.size;
  }

  /** distance between the leftmost and rightmost elements, both included; 0 when nothing is stored */
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
    return this._elements.has(index) ? (this._elements.get(index) as T) : this._default;
  }

  /**
   * inserts or updates the element at `index`, returning `this`
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  set(index: number, value: T): this {
    assertIndex(index);
    if (this._equals(value)) {
      if (this._elements.delete(index)) {
        this._invalidate();
      }
    } else {
      if (!this._elements.has(index)) {
        this._invalidate();
      }
      this._elements.set(index, value);
    }
    return this;
  }

  /**
   * resets the element at `index` to the default, returning whether there was one
   *
   * @throws {TypeError} If `index` is not an integer.
   */
  resetValue(index: number): boolean {
    const removed = this._elements.delete(assertIndex(index));
    if (removed) {
      this._invalidate();
    }
    return removed;
  }

  /** resets every element, keeping the default, returning whether there was any */
  resetVector(): boolean {
    if (this._elements.size === 0) {
      return false;
    }
    this._elements.clear();
    this._invalidate();
    return true;
  }

  /** every element, descending by index */
  elements(): SV_element<T>[] {
    return this._orderedIndices().map((index) => ({ index, value: this._stored(index) }));
  }

  /** every element, ascending by index */
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
   * the (n+1)-th element from the left, the ordinal numbering the elements and not the positions
   *
   * @throws {RangeError} If `n` is negative, or no explicit element sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  element(n: number): SV_element<T> {
    const index = this.elementIndex(n);
    return { index, value: this._stored(index) };
  }

  /**
   * the index {@link element} sits at
   *
   * @throws {RangeError} If `n` is negative, or no explicit element sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  elementIndex(n: number): number {
    return this._ordinalIndex(n, false);
  }

  /**
   * the value {@link element} holds
   *
   * @throws {RangeError} If `n` is negative, or no explicit element sits that far from the left.
   * @throws {TypeError} If `n` is not an integer.
   */
  elementValue(n: number): T {
    return this._stored(this.elementIndex(n));
  }

  /**
   * the (n+1)-th element from the right, `invertedElement(0)` being the rightmost
   *
   * @throws {RangeError} If `n` is negative, or no explicit element sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  invertedElement(n: number): SV_element<T> {
    const index = this.invertedElementIndex(n);
    return { index, value: this._stored(index) };
  }

  /**
   * the index {@link invertedElement} sits at
   *
   * @throws {RangeError} If `n` is negative, or no explicit element sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  invertedElementIndex(n: number): number {
    return this._ordinalIndex(n, true);
  }

  /**
   * the value {@link invertedElement} holds
   *
   * @throws {RangeError} If `n` is negative, or no explicit element sits that far from the right.
   * @throws {TypeError} If `n` is not an integer.
   */
  invertedElementValue(n: number): T {
    return this._stored(this.invertedElementIndex(n));
  }

  /**
   * the value `n` positions right of the leftmost element, empty positions counting; a negative `n`
   * walks left of it, into the default value
   *
   * @throws {RangeError} If no element is stored at all, so there is nothing to measure from.
   * @throws {TypeError} If `n` is not an integer.
   */
  leftSignificantValue(n: number): T {
    assertIndex(n);
    const first = this._anchor(false);
    return this.get(first - n);
  }

  /**
   * the value `n` positions left of the rightmost element, a negative `n` walking right of it
   *
   * @throws {RangeError} If no element is stored at all, so there is nothing to measure from.
   * @throws {TypeError} If `n` is not an integer.
   */
  rightSignificantValue(n: number): T {
    assertIndex(n);
    const last = this._anchor(true);
    return this.get(last + n);
  }

  /**
   * whether `other` holds the same values as this vector, every comparison made by this vector's
   * predicate: the two default values first, then each index either vector stores an element at, a
   * position holding nothing contributing its own vector's default
   *
   * Never throws. A mismatch of default-value types is simply not equal, as are two `NaN` defaults,
   * since `NaN !== NaN` — so a vector holding one is not equal to itself. The three properties of
   * equality hold only as far as the predicate does: an asymmetric or non-reflexive predicate
   * gives an asymmetric or non-reflexive answer, the library never correcting it.
   */
  isEqualTo(other: SV_vector<T>): boolean {
    if (typeof this._default !== typeof other._default) {
      return false;
    }
    if (!this._equalsValues(other._default, this._default)) {
      return false;
    }
    for (const index of this._unionIndexes(other)) {
      const mine = this._storedOrDefault(index);
      if (!this._equalsValues(other._storedOrDefault(index), mine)) {
        return false;
      }
    }
    return true;
  }

  /**
   * the elements of this vector that differ from `other`, descending by index, each carrying this
   * vector's value at that index — the default value itself wherever nothing is stored there
   *
   * Not a symmetric difference, and not guaranteed to be empty for `a.differences(a)`: a stored
   * `NaN` differs from itself, since `NaN !== NaN`.
   *
   * @throws {TypeError} If the two default values are not of the same type, or do not compare
   * equal under this vector's predicate.
   */
  differences(other: SV_vector<T>): SV_element<T>[] {
    if (typeof this._default !== typeof other._default) {
      throw new TypeError(
        `SV_vector default values are of different types, ${typeof this._default} and ` +
          `${typeof other._default}, so the two vectors cannot be compared`,
      );
    }
    if (!this._equalsValues(other._default, this._default)) {
      throw new TypeError(
        'SV_vector default values are not equal, so the two vectors cannot be compared',
      );
    }
    const differing: SV_element<T>[] = [];
    for (const index of this._unionIndexes(other)) {
      const value = this._storedOrDefault(index);
      if (!this._equalsValues(other._storedOrDefault(index), value)) {
        differing.push({ index, value });
      }
    }
    return differing;
  }

  /** every element, descending by index, read as the walk goes */
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
    copy._equality = this._equality;
    for (const [index, value] of this._elements) {
      copy._elements.set(index, value);
    }
    return copy;
  }

  /**
   * builds from any iterable of elements, dropping those equal to the default and keeping the
   * last of a repeated index
   *
   * @param elements elements to store, in any order
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

  /** the indices of every stored element, descending, or ascending when `inverted` */
  private _orderedIndices(inverted = false): number[] {
    const ascending = this._ascendingIndexes();
    return inverted ? ascending.slice() : ascending.slice().reverse();
  }

  /**
   * the indices either vector stores an element at, descending, each index once
   *
   * A merge of the two sorted index lists, so the union itself is never built.
   */
  private *_unionIndexes(other: SV_vector<T>): Generator<number> {
    const mine = this._ascendingIndexes();
    const theirs = other._ascendingIndexes();
    let left = mine.length - 1;
    let right = theirs.length - 1;
    while (left >= 0 || right >= 0) {
      const mineIndex = left >= 0 ? (mine[left] as number) : undefined;
      const theirIndex = right >= 0 ? (theirs[right] as number) : undefined;
      if (theirIndex === undefined || (mineIndex !== undefined && mineIndex > theirIndex)) {
        yield mineIndex as number;
        left -= 1;
      } else if (mineIndex === undefined || theirIndex > mineIndex) {
        yield theirIndex;
        right -= 1;
      } else {
        yield mineIndex as number;
        left -= 1;
        right -= 1;
      }
    }
  }

  /** every stored index, ascending, sorted once and kept until the elements change */
  private _ascendingIndexes(): number[] {
    let sorted = this._sortedIndexes;
    if (sorted === undefined) {
      sorted = [...this._elements.keys()].sort((a, b) => a - b);
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
        `SV_vector holds ${ascending.length} elements, so there is no element ${n} from the ${side}`,
      );
    }
    return index;
  }

  private _anchor(inverted: boolean): number {
    const ascending = this._ascendingIndexes();
    const index = inverted ? ascending[0] : ascending[ascending.length - 1];
    if (index === undefined) {
      throw new RangeError('SV_vector holds no elements, so there is nothing to measure from');
    }
    return index;
  }

  private _stored(index: number): T {
    return this._elements.get(index) as T;
  }

  /** the value at `index`, or the default when nothing is stored there, index taken as it is */
  private _storedOrDefault(index: number): T {
    return this._elements.has(index) ? (this._elements.get(index) as T) : this._default;
  }

  /**
   * whether `left` and `right` are equal, by the assigned predicate when there is one and `===`
   * otherwise
   *
   * The predicate is called `(left, right)` — which of the two values belongs in the first slot is
   * decided by the caller, and the receiver's value always goes second.
   */
  private _equalsValues(left: T, right: T): boolean {
    return this._equality === undefined ? left === right : this._equality(left, right);
  }

  /** whether `value` equals the default, by the assigned predicate or by `===` */
  private _equals(value: T): boolean {
    return this._equalsValues(value, this._default);
  }

  private _prune(): void {
    let pruned = false;
    for (const [index, value] of this._elements) {
      if (this._equals(value)) {
        this._elements.delete(index);
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

/** SV_vector ordinals number the elements, starting at 0 and never negative */
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
