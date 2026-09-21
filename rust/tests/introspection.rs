//! Mirrors the `introspection` block of the TypeScript suite.

use sparse_vector::{Element, SparseVector};

#[test]
fn lists_elements_in_ascending_index_order() {
    let mut vector = SparseVector::with_default("");
    vector.set(5, "e");
    vector.set(-2, "b");
    vector.set(0, "c");
    assert_eq!(
        vector.elements(),
        [
            Element {
                index: -2,
                value: "b"
            },
            Element {
                index: 0,
                value: "c"
            },
            Element {
                index: 5,
                value: "e"
            },
        ]
    );
    assert_eq!(vector.keys(), [-2, 0, 5]);
    assert_eq!(vector.values(), ["b", "c", "e"]);
}

#[test]
fn is_iterable() {
    let mut vector = SparseVector::with_default("");
    vector.set(2, "b");
    vector.set(1, "a");

    let mut seen = Vec::new();
    for element in &vector {
        seen.push((element.index, *element.value));
    }

    assert_eq!(seen, [(1, "a"), (2, "b")]);
}

#[test]
fn serialises_to_its_explicit_entries_only() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");
    vector.set(2, ""); // equal to the default, so never stored
    assert_eq!(
        vector.elements(),
        [Element {
            index: 1,
            value: "a"
        }]
    );
}

#[test]
fn clones_independently() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");
    let mut copy = vector.clone();
    copy.set(2, "b");
    copy.set_default_value("nine");

    assert_eq!(
        vector.elements(),
        [Element {
            index: 1,
            value: "a"
        }]
    );
    assert_eq!(vector.default_value(), &"");
    assert_eq!(copy.len(), 2);
    assert_eq!(copy.default_value(), &"nine");
}

// No TypeScript counterpart. Fails the day someone swaps the storage for a
// HashMap, because ascending order is a documented guarantee.
#[test]
fn keeps_ascending_order_under_adversarial_insertion() {
    let mut vector = SparseVector::with_default("");
    for index in (0_i64..50).rev() {
        vector.set(index, "x");
    }

    let keys = vector.keys();
    assert!(keys.windows(2).all(|pair| pair[0] < pair[1]));
    assert_eq!(keys.first(), Some(&0));
    assert_eq!(keys.last(), Some(&49));
}

// No TypeScript counterpart: its indices are doubles capped at 2^53-1.
#[test]
fn supports_the_whole_i64_index_range() {
    let mut vector = SparseVector::new();
    vector.set(i64::MIN, 1.0);
    vector.set(i64::MAX, 2.0);

    assert_eq!(vector.get(i64::MIN), 1.0);
    assert_eq!(vector.get(i64::MAX), 2.0);
    assert_eq!(vector.len(), 2);
}
