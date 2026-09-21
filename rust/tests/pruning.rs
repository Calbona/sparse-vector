//! Mirrors the `pruning of default-valued entries` block of the TypeScript suite.

use sparse_vector::{Element, SparseVector};

#[test]
fn never_stores_a_value_equal_to_the_default() {
    let mut vector = SparseVector::new();
    vector.set(1, 0.0);
    assert_eq!(vector.len(), 0);
    assert!(!vector.contains_key(1));
}

#[test]
fn prunes_an_entry_that_becomes_the_default() {
    let mut vector = SparseVector::new();
    vector.set(1, 5.0);
    assert_eq!(vector.len(), 1);

    vector.set(1, 0.0);
    assert_eq!(vector.len(), 0);
    assert_eq!(vector.get(1), 0.0);
}

#[test]
fn prunes_on_a_change_of_default_and_forgets_pruned_positions() {
    let mut vector: SparseVector<Option<i32>> = SparseVector::with_default(Some(0));
    vector.set(1, None);
    vector.set(2, Some(7));
    assert_eq!(
        vector.elements(),
        [
            Element {
                index: 1,
                value: None
            },
            Element {
                index: 2,
                value: Some(7)
            },
        ]
    );

    vector.set_default_value(None);
    // Position 1 was explicitly None and is dropped; position 2 keeps its value.
    assert_eq!(
        vector.elements(),
        [Element {
            index: 2,
            value: Some(7)
        }]
    );
    // The old default is not remembered: position 3 reads back as None.
    assert_eq!(vector.get(3), None);
    assert_eq!(vector.len(), 1);
}

#[test]
fn keeps_entries_that_were_pruned_earlier_gone() {
    let mut vector: SparseVector<Option<i32>> = SparseVector::with_default(Some(0));
    vector.set(1, Some(0));
    vector.set(2, Some(7));
    assert!(!vector.contains_key(1));

    vector.set_default_value(None);
    vector.set_default_value(Some(0));
    assert_eq!(
        vector.elements(),
        [Element {
            index: 2,
            value: Some(7)
        }]
    );
}

// Stands in for the TypeScript case `compares strictly`, which stores 0, '0',
// false and null: four distinct types there, impossible to share one T here.
// Only an exact match drops an entry, so the stand-in needs values that differ
// while looking alike.
#[test]
fn compares_exactly() {
    let mut vector = SparseVector::with_default("0");
    vector.set(1, "00");
    vector.set(2, "0 ");
    vector.set(3, "");
    assert_eq!(vector.len(), 3);
}

#[test]
fn documents_the_nan_caveat() {
    let mut vector = SparseVector::with_default(f64::NAN);
    vector.set(1, f64::NAN);
    // NaN != NaN, so comparison cannot recognise it as the default.
    assert_eq!(vector.len(), 1);
}

// No TypeScript counterpart, and a real divergence: `PartialEq` on a user type
// is structural where `===` is referential. The README gives the recipe for
// identity when that is what you want.
#[test]
fn prunes_by_value_not_by_identity() {
    #[derive(Clone, Debug, PartialEq)]
    struct Widget {
        a: i32,
    }

    let mut vector: SparseVector<Widget> = SparseVector::with_default(Widget { a: 0 });
    vector.set(1, Widget { a: 0 }); // equal to the default, but a distinct instance
    assert_eq!(vector.len(), 0);

    vector.set(2, Widget { a: 1 });
    assert_eq!(vector.len(), 1);
}

#[test]
fn prunes_a_negative_zero() {
    // -0.0 == 0.0, and TypeScript's -0 === 0 does the same.
    let mut vector = SparseVector::new();
    vector.set(1, -0.0);
    assert_eq!(vector.len(), 0);
}
