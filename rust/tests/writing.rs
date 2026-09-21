//! Mirrors the `writing` block of the TypeScript suite.

use sparse_vector::SparseVector;

#[test]
fn inserts_and_updates() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");
    assert_eq!(vector.get(1), "a");
    assert_eq!(vector.len(), 1);

    vector.set(1, "b");
    assert_eq!(vector.get(1), "b");
    assert_eq!(vector.len(), 1);
}

#[test]
fn deletes_to_the_default_value() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");
    // `remove` returns the value rather than a bool; `.is_some()` recovers it.
    assert_eq!(vector.remove(1), Some("a"));
    assert_eq!(vector.get(1), "");
    assert_eq!(vector.len(), 0);
    assert_eq!(vector.remove(1), None);
}

#[test]
fn clears_every_entry_but_keeps_the_default() {
    let mut vector = SparseVector::with_default("gone");
    vector.set(1, "a");
    vector.set(-2, "b");
    vector.clear();
    assert_eq!(vector.len(), 0);
    assert_eq!(vector.default_value(), &"gone");
    assert_eq!(vector.get(1), "gone");
}

#[test]
fn chains_writes() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a").set(2, "b").set(3, "c");
    assert_eq!(vector.keys(), [1, 2, 3]);
}
