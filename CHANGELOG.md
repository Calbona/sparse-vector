# Changelog

## 3.0.0 — 2026-10-07

### Added

A settable equality predicate, in all three implementations. It takes the value under test and the current default value, returns a boolean, and replaces the language's ordinary comparison everywhere the default value is compared, pruning included.

Whole-vector comparison, in all three implementations: `isEqualTo` / `is_equal_to` answers whether two vectors are equal, and `differences` lists the elements where the receiver diverges from the other. Both answer under the receiver's predicate, so two vectors carrying different predicates can disagree about who equals whom.

### Changed

Writing and resetting now report what happened: resetting an index reports whether there was anything there to reset, resetting the vector reports whether it held anything at all, and replacing the default value reports whether anything was pruned.

The Rust constructors are now `SparseVector::new(default)` and `SparseVector::default_new()`; `with_default` is gone, and `default_new` draws on any `T: Default` rather than being implemented for `f64` alone.

What the vector stores is now called an element everywhere — the documentation in all ten languages and the source comments alike.

The C++ implementation stores its elements unordered and serves the descending order it promises from an index rebuilt on demand. Reading one vector from two threads at once is no longer safe, and `begin()`, `end()` and `get_plus_dimension()` no longer promise not to throw.

### Version bumps

- `typescript-v3.0.0`
- `cpp-v3.0.0`
- `rust-v3.0.0`

## 2.0.0 — 2026-09-24

### Changed

The entries now list the way a number is written by hand — the largest index leftmost — reversing every listing and swapping the end the ordinal and significant-value methods count from.

### Version bumps

- `typescript-v2.0.0`
- `cpp-v2.0.0`
- `rust-v2.0.0`

## 1.0.0 — 2026-09-22

### Added

Eight query methods on `SV_vector` in all three implementations: reaching a stored entry by its ordinal from either end, and measuring a position relative to the outermost stored entries.

### Version bumps

- `typescript-v1.1.0`
- `cpp-v1.1.0`
- `rust-v1.1.0`
