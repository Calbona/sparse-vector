//! Mirrors the `default value` block of the TypeScript suite.

use sparse_vector::SparseVector;

#[test]
fn defaults_to_the_number_0_when_omitted() {
    // The default type parameter does not pin T here, so the spelling is explicit.
    let vector: SparseVector<f64> = SparseVector::default_new();
    assert_eq!(vector.get_default_value(), &0.0);
    assert_eq!(vector.get(5), 0.0);
}

#[test]
fn accepts_an_explicit_default() {
    let vector = SparseVector::new("");
    assert_eq!(vector.get(5), "");
    assert_eq!(vector.get(-5), "");
}

// Stands in for the TypeScript case `honours an explicit undefined default`.
// Rust has no `undefined`, so the counterpart is a T that is itself the "nothing"
// value; the original case's intent, a default that is not a number, survives.
#[test]
fn honours_an_explicit_optional_default() {
    let vector: SparseVector<Option<i32>> = SparseVector::new(None);
    assert_eq!(vector.get_default_value(), &None);
    assert_eq!(vector.get(0), None);
}

#[test]
fn is_replaceable_after_construction() {
    let mut vector = SparseVector::default_new();
    vector.set(1, 42.0);
    assert_eq!(vector.get(2), 0.0);

    vector.set_default_value(-1.0);
    assert_eq!(vector.get(2), -1.0);
    assert_eq!(vector.get(1), 42.0);
}

// The T-generic spelling of `new(T::default())`, for a T that has no literal `0`.
#[test]
fn generalises_to_any_defaultable_type() {
    let vector: SparseVector<String> = SparseVector::default_new();
    assert_eq!(vector.get_default_value(), "");
    assert_eq!(vector.get(5), "");
}
