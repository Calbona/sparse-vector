//! Mirrors the `introspection` block of the TypeScript suite.

use sparse_vector::{Element, SparseVector};

#[test]
fn lists_elements_in_descending_index_order() {
    let mut vector = SparseVector::with_default("");
    vector.set(5, "e");
    vector.set(-2, "b");
    vector.set(0, "c");
    assert_eq!(
        vector.elements(),
        [
            Element {
                index: 5,
                value: "e"
            },
            Element {
                index: 0,
                value: "c"
            },
            Element {
                index: -2,
                value: "b"
            },
        ]
    );
    assert_eq!(vector.indexes(), [5, 0, -2]);
    assert_eq!(vector.values(), ["e", "c", "b"]);
}

#[test]
fn lists_the_inverted_views_in_ascending_index_order() {
    let mut vector = SparseVector::with_default("");
    vector.set(5, "e");
    vector.set(-2, "b");
    vector.set(0, "c");

    assert_eq!(
        vector.inverted_elements(),
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
    assert_eq!(vector.inverted_indexes(), [-2, 0, 5]);
    assert_eq!(vector.inverted_values(), ["b", "c", "e"]);

    // The ordinal trio numbers the inverted listing: inverted_element(n) is
    // inverted_elements()[n], read from its first entry.
    assert_eq!(
        vector.inverted_indexes()[0],
        vector.inverted_elements()[0].index
    );
    assert_eq!(vector.inverted_element(1), vector.inverted_elements()[1]);
}

#[test]
fn reports_the_entry_count() {
    let mut vector = SparseVector::with_default("");
    assert_eq!(vector.get_element_amount(), 0);

    vector.set(1, "a");
    assert_eq!(vector.get_element_amount(), 1);

    vector.set(1, ""); // equal to the default, so dropped again
    assert_eq!(vector.get_element_amount(), 0);
}

#[test]
fn measures_the_span_between_the_outermost_entries() {
    let mut vector = SparseVector::with_default("");
    // Nothing stored spans nothing.
    assert_eq!(vector.get_significant_dimension(), 0);

    vector.set(-3, "a");
    // A single entry spans itself.
    assert_eq!(vector.get_significant_dimension(), 1);

    // -2 through 5 inclusive: the positions in between count, so this is not the
    // entry count.
    let mut span = SparseVector::with_default("");
    span.set(5, "e");
    span.set(-2, "b");
    assert_eq!(span.get_significant_dimension(), 8);
}

#[test]
fn measures_how_far_the_vector_reaches_each_side_of_zero() {
    let vector = SparseVector::with_default("");
    // Nothing stored reaches nowhere.
    assert_eq!(vector.get_plus_dimension(), 0);
    assert_eq!(vector.get_minus_dimension(), 0);

    // Entries on one side only leave the other side at zero.
    let mut positive = SparseVector::with_default("");
    positive.set(3, "a");
    positive.set(5, "b");
    assert_eq!(positive.get_plus_dimension(), 5);
    assert_eq!(positive.get_minus_dimension(), 0);

    let mut negative = SparseVector::with_default("");
    negative.set(-3, "a");
    negative.set(-5, "b");
    assert_eq!(negative.get_plus_dimension(), 0);
    assert_eq!(negative.get_minus_dimension(), 5);

    // Straddling zero: 5 above, 2 below, and 8 positions from end to end.
    let mut span = SparseVector::with_default("");
    span.set(5, "e");
    span.set(-2, "b");
    assert_eq!(span.get_plus_dimension(), 5);
    assert_eq!(span.get_minus_dimension(), 2);
    assert_eq!(span.get_significant_dimension(), 8);
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

    assert_eq!(seen, [(2, "b"), (1, "a")]);
}

// No TypeScript or C++ counterpart: neither of those iterators is double-ended.
#[test]
fn walks_from_either_end() {
    let mut vector = SparseVector::with_default("");
    vector.set(-2, "b");
    vector.set(0, "c");
    vector.set(5, "e");

    let ascending: Vec<i64> = vector.iter().rev().map(|element| element.index).collect();
    assert_eq!(ascending, [-2, 0, 5]);

    // The two ends meet in the middle, the count staying exact as they go.
    let mut walk = vector.iter();
    assert_eq!(walk.next().map(|element| element.index), Some(5));
    assert_eq!(walk.next_back().map(|element| element.index), Some(-2));
    assert_eq!(walk.len(), 1);
    assert_eq!(walk.next().map(|element| element.index), Some(0));
    assert_eq!(walk.next(), None);
    assert_eq!(walk.next_back(), None);
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
    assert_eq!(vector.get_default_value(), &"");
    assert_eq!(copy.get_element_amount(), 2);
    assert_eq!(copy.get_default_value(), &"nine");
}

// No TypeScript counterpart. Fails the day someone swaps the storage for a
// HashMap, because descending order is a documented guarantee.
#[test]
fn keeps_descending_order_under_adversarial_insertion() {
    let mut vector = SparseVector::with_default("");
    for index in 0_i64..50 {
        vector.set(index, "x");
    }

    let indexes = vector.indexes();
    assert!(indexes.windows(2).all(|pair| pair[0] > pair[1]));
    assert_eq!(indexes.first(), Some(&49));
    assert_eq!(indexes.last(), Some(&0));
}

// No TypeScript counterpart: its indices are doubles capped at 2^53-1.
#[test]
fn supports_the_whole_i64_index_range() {
    let mut vector = SparseVector::new();
    vector.set(i64::MIN, 1.0);
    vector.set(i64::MAX, 2.0);

    assert_eq!(vector.get(i64::MIN), 1.0);
    assert_eq!(vector.get(i64::MAX), 2.0);
    assert_eq!(vector.get_element_amount(), 2);
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
            index: 5,
            value: "e"
        }
    );
    assert_eq!(
        vector.element(2),
        Element {
            index: -2,
            value: "b"
        }
    );
    assert_eq!(vector.element_index(1), 0);
    assert_eq!(vector.element_value(1), "c");
}

#[test]
fn counts_from_the_inverted_end_by_ordinal() {
    let mut vector = SparseVector::with_default("");
    vector.set(-2, "b");
    vector.set(0, "c");
    vector.set(5, "e");

    assert_eq!(
        vector.inverted_element(0),
        Element {
            index: -2,
            value: "b"
        }
    );
    assert_eq!(
        vector.inverted_element(2),
        Element {
            index: 5,
            value: "e"
        }
    );
    assert_eq!(vector.inverted_element_index(1), 0);
    assert_eq!(vector.inverted_element_value(1), "c");
}

#[test]
fn refuses_an_ordinal_without_a_matching_entry() {
    let mut vector = SparseVector::with_default("");
    vector.set(1, "a");

    // One entry stored, so ordinal 0 reaches it and ordinal 1 is past the end.
    assert!(panics(|| { vector.element(1); }));
    assert!(panics(|| { vector.element_index(1); }));
    assert!(panics(|| { vector.element_value(1); }));
    assert!(panics(|| { vector.inverted_element(1); }));
    assert!(panics(|| { vector.inverted_element_index(1); }));
    assert!(panics(|| { vector.inverted_element_value(1); }));

    let none: SparseVector<&str> = SparseVector::with_default("");
    assert!(panics(|| { none.element(0); }));
    assert!(panics(|| { none.inverted_element(0); }));
}

#[test]
fn measures_significant_positions_from_the_first_entry() {
    let mut vector = SparseVector::with_default("");
    vector.set(10, "a");
    vector.set(13, "d");

    // The leftmost entry is the most significant digit.
    assert_eq!(vector.left_significant_value(0), "d");
    // Positions holding no entry count, like the zeros inside a number.
    assert_eq!(vector.left_significant_value(2), "");
    assert_eq!(vector.left_significant_value(3), "a");
    // A negative offset walks off the left of that first entry, into the default.
    assert_eq!(vector.left_significant_value(-1), "");
}

#[test]
fn measures_significant_positions_from_the_last_entry() {
    let mut vector = SparseVector::with_default("");
    vector.set(10, "a");
    vector.set(13, "d");

    assert_eq!(vector.right_significant_value(0), "a");
    assert_eq!(vector.right_significant_value(3), "d");
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
    // One entry, anchoring both methods: one step below it is reachable from
    // either end, one step above it is not.
    assert_eq!(high.left_significant_value(0), 1.0);
    assert_eq!(high.left_significant_value(1), 0.0);
    assert_eq!(high.right_significant_value(-1), 0.0);
    assert!(panics(|| { high.left_significant_value(-1); }));
    assert!(panics(|| { high.right_significant_value(1); }));

    let mut low = SparseVector::new();
    low.set(i64::MIN, 2.0);
    assert_eq!(low.right_significant_value(0), 2.0);
    assert_eq!(low.right_significant_value(1), 0.0);
    assert_eq!(low.left_significant_value(-1), 0.0);
    assert!(panics(|| { low.right_significant_value(-1); }));
    assert!(panics(|| { low.left_significant_value(1); }));
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
