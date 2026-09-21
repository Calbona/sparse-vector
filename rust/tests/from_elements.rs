//! Mirrors the `SV_vector.from` block of the TypeScript suite.

use sparse_vector::{Element, SparseVector};

#[test]
fn builds_from_elements_dropping_defaults_and_favouring_the_last_duplicate() {
    let vector = SparseVector::from_elements(
        [
            Element {
                index: 3,
                value: "c",
            },
            Element {
                index: 1,
                value: "a",
            },
            Element {
                index: 3,
                value: "last",
            },
            Element {
                index: 4,
                value: "",
            },
        ],
        "",
    );

    assert_eq!(
        vector.elements(),
        [
            Element {
                index: 1,
                value: "a"
            },
            Element {
                index: 3,
                value: "last"
            },
        ]
    );
}

#[test]
fn takes_a_default_value() {
    let vector = SparseVector::from_elements(
        [Element {
            index: 1,
            value: "x",
        }],
        "z",
    );
    assert_eq!(vector.default_value(), &"z");
    assert_eq!(vector.get(99), "z");
}

// Stands in for the TypeScript case `round-trips through JSON when the default is
// carried alongside`. There is no JSON encoder here, so the round trip goes
// through elements(), which is the contract.
#[test]
fn round_trips_when_the_default_is_carried_alongside() {
    let mut original = SparseVector::with_default("none");
    original.set(-1, "a");
    original.set(10, "b");

    let restored = SparseVector::from_elements(original.elements(), "none");

    assert_eq!(restored.elements(), original.elements());
    assert_eq!(restored.get(0), "none");
}

// The TypeScript signature takes any Iterable; this takes any IntoIterator.
#[test]
fn accepts_any_into_iterator() {
    let vector = SparseVector::from_elements(
        (0_i64..4).map(|index| Element {
            index,
            value: index * 2,
        }),
        0,
    );

    assert_eq!(vector.len(), 3); // index 0 holds 0, which is the default
    assert_eq!(vector.keys(), [1, 2, 3]);
}
