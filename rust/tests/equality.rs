//! Mirrors the `equality` block of the TypeScript suite.

use std::sync::Arc;

use sparse_vector::{Element, Equality, SparseVector};

/// Equality by length, standing in for a notion `PartialEq` cannot express.
fn by_length() -> Equality<String> {
    Arc::new(|value: &String, default: &String| value.len() == default.len())
}

#[test]
fn replaces_the_ordinary_comparison() {
    let mut vector = SparseVector::new("ab".to_string());
    vector.set_equality(Some(by_length()));
    vector.set(1, "xy".to_string()); // a different string of the same length
    assert_eq!(vector.get_element_amount(), 0);
}

#[test]
fn prunes_what_the_new_predicate_calls_equal() {
    let mut vector = SparseVector::new("ab".to_string());
    vector.set(1, "xy".to_string());
    vector.set(2, "x".to_string());
    assert_eq!(vector.get_element_amount(), 2);

    assert!(vector.set_equality(Some(by_length())));
    assert_eq!(
        vector.elements(),
        [Element {
            index: 2,
            value: "x".to_string()
        }]
    );
}

#[test]
fn reports_the_predicate() {
    let vector = SparseVector::new(String::new());
    assert!(vector.get_equality().is_none());
}

#[test]
fn restores_the_ordinary_comparison() {
    let mut vector = SparseVector::new("ab".to_string());
    vector.set_equality(Some(by_length()));
    vector.set(1, "xy".to_string());
    assert_eq!(vector.get_element_amount(), 0);

    vector.set_equality(None);
    vector.set(1, "xy".to_string());
    assert_eq!(vector.get_element_amount(), 1);
}

#[test]
fn travels_with_a_clone() {
    let mut vector = SparseVector::new("ab".to_string());
    vector.set_equality(Some(by_length()));

    let mut copy = vector.clone();
    assert!(copy.get_equality().is_some());
    copy.set(1, "xy".to_string());
    assert_eq!(copy.get_element_amount(), 0);
}

/// Not transitive: one is within one of another, and of a third, without the first and third
/// having anything to do with each other.
fn within_one() -> Equality<f64> {
    Arc::new(|value: &f64, default: &f64| (value - default).abs() <= 1.0)
}

#[test]
fn compares_the_two_default_values() {
    let a = SparseVector::new(0.0);
    assert!(!a.is_equal_to(&SparseVector::new(5.0)));
    assert!(a.is_equal_to(&SparseVector::new(0.0)));
}

#[test]
fn compares_every_element() {
    let mut a = SparseVector::new(0.0);
    let mut b = SparseVector::new(0.0);
    a.set(1, 7.0);
    b.set(1, 7.0);
    assert!(a.is_equal_to(&b));

    b.set(2, 7.0);
    assert!(!a.is_equal_to(&b));
}

#[test]
fn treats_a_missing_position_as_the_default() {
    let a = SparseVector::new(0.0);
    let mut b = SparseVector::new(0.0);
    b.set(3, 9.0);
    assert!(!a.is_equal_to(&b));
    assert_eq!(a.differences(&b), [Element { index: 3, value: 0.0 }]);
}

#[test]
fn compares_across_different_default_values() {
    let mut a = SparseVector::new(0.0);
    let mut b = SparseVector::new(1.0);
    a.set_equality(Some(within_one()));
    b.set_equality(Some(within_one()));
    b.set(4, -1.0); // further than one from b's default, so it stays; within one of a's

    assert_eq!(a.get_element_amount(), 0);
    assert_eq!(b.get_element_amount(), 1);
    assert!(a.is_equal_to(&b));
    assert!(a.differences(&b).is_empty());
}

#[test]
fn lists_what_differs_descending() {
    let mut a = SparseVector::new(0.0);
    a.set(1, 10.0);
    a.set(3, 30.0);
    a.set(5, 50.0);
    let mut b = SparseVector::new(0.0);
    b.set(3, 30.0); // the one they agree on

    assert_eq!(
        a.differences(&b),
        [
            Element {
                index: 5,
                value: 50.0
            },
            Element {
                index: 1,
                value: 10.0
            }
        ]
    );
    assert!(!a.is_equal_to(&b));
}

#[test]
fn carries_the_default_value_for_a_position_only_the_other_stores() {
    let a = SparseVector::new(0.0);
    let mut b = SparseVector::new(0.0);
    b.set(2, 7.0);
    assert_eq!(a.differences(&b), [Element { index: 2, value: 0.0 }]);
}

#[test]
#[should_panic(expected = "default values are not equal")]
fn refuses_to_list_differences_against_an_unequal_default() {
    let a = SparseVector::new(0.0);
    let b = SparseVector::new(5.0);
    let _ = a.differences(&b);
}

#[test]
fn documents_the_nan_caveat_for_whole_vector_comparison() {
    let nan = SparseVector::new(f64::NAN);
    assert!(!nan.is_equal_to(&nan)); // NaN is not equal to itself

    let mut stored = SparseVector::new(0.0);
    stored.set(1, f64::NAN); // kept, since NaN != NaN
    assert!(!stored.is_equal_to(&stored));

    let differing = stored.differences(&stored);
    assert_eq!(differing.len(), 1);
    assert_eq!(differing[0].index, 1);
    assert!(differing[0].value.is_nan());
}

#[test]
#[should_panic(expected = "default values are not equal")]
fn refuses_to_list_differences_against_a_nan_default() {
    let nan = SparseVector::new(f64::NAN);
    let _ = nan.differences(&nan);
}
