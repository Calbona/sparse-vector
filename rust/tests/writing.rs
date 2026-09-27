//! Mirrors the `writing` block of the TypeScript suite.

use sparse_vector::SparseVector;

#[test]
fn inserts_and_updates() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");
    assert_eq!(vector.get(1), "a");
    assert_eq!(vector.get_element_amount(), 1);

    vector.set(1, "b");
    assert_eq!(vector.get(1), "b");
    assert_eq!(vector.get_element_amount(), 1);
}

#[test]
fn resets_to_the_default_value() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");
    // `reset_value` returns the value rather than a bool; `.is_some()` recovers it.
    assert_eq!(vector.reset_value(1), Some("a"));
    assert_eq!(vector.get(1), "");
    assert_eq!(vector.get_element_amount(), 0);
    assert_eq!(vector.reset_value(1), None);
}

#[test]
fn resets_every_entry_but_keeps_the_default() {
    let mut vector = SparseVector::with_default("gone");
    vector.set(1, "a");
    vector.set(-2, "b");
    vector.reset_vector();
    assert_eq!(vector.get_element_amount(), 0);
    assert_eq!(vector.get_default_value(), &"gone");
    assert_eq!(vector.get(1), "gone");
}

#[test]
fn chains_writes() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a").set(2, "b").set(3, "c");
    assert_eq!(vector.indexes(), [3, 2, 1]);
}
