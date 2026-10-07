//! Mirrors the `pruning of default-valued elements` block of the TypeScript suite.

use sparse_vector::{Element, SparseVector};

#[test]
fn never_stores_a_value_equal_to_the_default() {
    let mut vector = SparseVector::default_new();
    vector.set(1, 0.0);
    assert_eq!(vector.get_element_amount(), 0);
    assert!(vector.elements().is_empty());
}

#[test]
fn prunes_an_element_that_becomes_the_default() {
    let mut vector = SparseVector::default_new();
    vector.set(1, 5.0);
    assert_eq!(vector.get_element_amount(), 1);

    vector.set(1, 0.0);
    assert_eq!(vector.get_element_amount(), 0);
    assert_eq!(vector.get(1), 0.0);
}

#[test]
fn prunes_on_a_change_of_default_and_forgets_pruned_positions() {
    let mut vector: SparseVector<Option<i32>> = SparseVector::new(Some(0));
    vector.set(1, None);
    vector.set(2, Some(7));
    assert_eq!(
        vector.elements(),
        [
            Element {
                index: 2,
                value: Some(7)
            },
            Element {
                index: 1,
                value: None
            },
        ]
    );

    assert!(vector.set_default_value(None));
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
    assert_eq!(vector.get_element_amount(), 1);
}

#[test]
fn keeps_elements_that_were_pruned_earlier_gone() {
    let mut vector: SparseVector<Option<i32>> = SparseVector::new(Some(0));
    vector.set(1, Some(0));
    vector.set(2, Some(7));
    assert_eq!(vector.indexes(), [2]);

    // Position 1 was dropped the moment it was written, so neither default prunes anything.
    assert!(!vector.set_default_value(None));
    assert!(!vector.set_default_value(Some(0)));
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
// Only an exact match drops an element, so the stand-in needs values that differ
// while looking alike.
#[test]
fn compares_exactly() {
    let mut vector = SparseVector::new("0");
    vector.set(1, "00");
    vector.set(2, "0 ");
    vector.set(3, "");
    assert_eq!(vector.get_element_amount(), 3);
}

#[test]
fn documents_the_nan_caveat() {
    let mut vector = SparseVector::new(f64::NAN);
    vector.set(1, f64::NAN);
    // NaN != NaN, so comparison cannot recognise it as the default.
    assert_eq!(vector.get_element_amount(), 1);
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

    let mut vector: SparseVector<Widget> = SparseVector::new(Widget { a: 0 });
    vector.set(1, Widget { a: 0 }); // equal to the default, but a distinct instance
    assert_eq!(vector.get_element_amount(), 0);

    vector.set(2, Widget { a: 1 });
    assert_eq!(vector.get_element_amount(), 1);
}

#[test]
fn prunes_a_negative_zero() {
    // -0.0 == 0.0, and TypeScript's -0 === 0 does the same.
    let mut vector = SparseVector::default_new();
    vector.set(1, -0.0);
    assert_eq!(vector.get_element_amount(), 0);
}
