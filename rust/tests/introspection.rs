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

#[test]
fn reaches_an_entry_by_ordinal() {
    let mut vector = SparseVector::with_default("");
    vector.set(-2, "b");
    vector.set(0, "c");
    vector.set(5, "e");

    // Ordinals number the entries, not the positions: 0 is the leftmost stored
    // entry however far out its index lies.
    assert_eq!(
        vector.element(0),
        Element {
            index: -2,
            value: "b"
        }
    );
    assert_eq!(
        vector.element(2),
        Element {
            index: 5,
            value: "e"
        }
    );
    assert_eq!(vector.element_index(1), 0);
    assert_eq!(vector.element_value(1), "c");
}

#[test]
fn counts_from_the_right_by_ordinal() {
    let mut vector = SparseVector::with_default("");
    vector.set(-2, "b");
    vector.set(0, "c");
    vector.set(5, "e");

    assert_eq!(
        vector.last_element(0),
        Element {
            index: 5,
            value: "e"
        }
    );
    assert_eq!(
        vector.last_element(2),
        Element {
            index: -2,
            value: "b"
        }
    );
    assert_eq!(vector.last_element_index(1), 0);
    assert_eq!(vector.last_element_value(1), "c");
}

#[test]
fn refuses_an_ordinal_without_a_matching_entry() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");

    // One entry stored, so ordinal 0 reaches it and ordinal 1 is already past the
    // end, wherever that entry's index happens to lie.
    assert!(panics(|| { vector.element(1); }));
    assert!(panics(|| { vector.element_index(1); }));
    assert!(panics(|| { vector.element_value(1); }));
    assert!(panics(|| { vector.last_element(1); }));
    assert!(panics(|| { vector.last_element_index(1); }));
    assert!(panics(|| { vector.last_element_value(1); }));

    let none: SparseVector<&str> = SparseVector::with_default("");
    assert!(panics(|| { none.element(0); }));
    assert!(panics(|| { none.last_element(0); }));
}

#[test]
fn measures_significant_positions_from_the_first_entry() {
    let mut vector = SparseVector::with_default("");
    vector.set(10, "a");
    vector.set(13, "d");

    assert_eq!(vector.left_significant_value(0), "a");
    // Positions holding no entry count, the way the zeros inside a number count.
    assert_eq!(vector.left_significant_value(2), "");
    assert_eq!(vector.left_significant_value(3), "d");
    // A negative offset walks off the left of that first entry, into the default.
    assert_eq!(vector.left_significant_value(-1), "");
}

#[test]
fn measures_significant_positions_from_the_last_entry() {
    let mut vector = SparseVector::with_default("");
    vector.set(10, "a");
    vector.set(13, "d");

    assert_eq!(vector.right_significant_value(0), "d");
    assert_eq!(vector.right_significant_value(3), "a");
    assert_eq!(vector.right_significant_value(4), "");
    assert_eq!(vector.right_significant_value(-1), "");
}

#[test]
fn refuses_a_significant_position_without_an_anchor() {
    let none: SparseVector<&str> = SparseVector::with_default("");

    assert!(panics(|| { none.left_significant_value(0); }));
    assert!(panics(|| { none.right_significant_value(0); }));
}

// No TypeScript counterpart: its indices are doubles, so a step past
// Number.MAX_SAFE_INTEGER loses precision rather than leaving the range.
#[test]
fn handles_a_position_at_the_edge_of_the_index_range() {
    let mut high = SparseVector::new();
    high.set(i64::MAX, 1.0);
    // The only entry is the anchor, so a step to its right would leave i64.
    assert_eq!(high.left_significant_value(0), 1.0);
    assert_eq!(high.left_significant_value(-1), 0.0);
    assert!(panics(|| { high.left_significant_value(1); }));

    let mut low = SparseVector::new();
    low.set(i64::MIN, 2.0);
    assert_eq!(low.right_significant_value(0), 2.0);
    assert_eq!(low.right_significant_value(-1), 0.0);
    assert!(panics(|| { low.right_significant_value(1); }));
}

/// Whether `throw` panics.
///
/// An out-of-range ordinal is a panic here, so a case asserting one has to catch
/// it. The hook is silenced for the duration, so a passing run stays quiet.
fn panics(throw: impl FnOnce() + std::panic::UnwindSafe) -> bool {
    let hook = std::panic::take_hook();
    std::panic::set_hook(Box::new(|_| {}));
    let caught = std::panic::catch_unwind(throw).is_err();
    std::panic::set_hook(hook);
    caught
}
