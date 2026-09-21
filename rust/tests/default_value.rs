//! Mirrors the `default value` block of the TypeScript suite.

use sparse_vector::SparseVector;

#[test]
fn defaults_to_the_number_0_when_omitted() {
    let vector = SparseVector::new();
    assert_eq!(vector.default_value(), &0.0);
    assert_eq!(vector.get(5), 0.0);
}

#[test]
fn accepts_an_explicit_default() {
    let vector = SparseVector::with_default("");
    assert_eq!(vector.get(5), "");
    assert_eq!(vector.get(-5), "");
}

// Stands in for the TypeScript case `honours an explicit undefined default`.
// Rust has no `undefined`, so the counterpart is a T that is itself the "nothing"
// value; the original case's intent, a default that is not a number, survives.
#[test]
fn honours_an_explicit_optional_default() {
    let vector: SparseVector<Option<i32>> = SparseVector::with_default(None);
    assert_eq!(vector.default_value(), &None);
    assert_eq!(vector.get(0), None);
}

#[test]
fn is_replaceable_after_construction() {
    let mut vector = SparseVector::new();
    vector.set(1, 42.0);
    assert_eq!(vector.get(2), 0.0);

    vector.set_default_value(-1.0);
    assert_eq!(vector.get(2), -1.0);
    assert_eq!(vector.get(1), 42.0);
}

// No TypeScript counterpart: `Default` is the T-generic spelling of
// with_default(T::default()), for a T that has no literal `0`.
#[test]
fn generalises_to_any_defaultable_type() {
    let vector: SparseVector<String> = SparseVector::default();
    assert_eq!(vector.default_value(), "");
    assert_eq!(vector.get(5), "");
}
