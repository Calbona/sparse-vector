/**
 * Decides whether a value equals the default value, replacing `===`.
 *
 * Called with the value under test first and the current default second.
 */
export type SV_equality<T = number> = (value: T, defaultValue: T) => boolean;
