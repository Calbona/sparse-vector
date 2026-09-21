//! Mirrors the `reading` block of the TypeScript suite.

use sparse_vector::SparseVector;

#[test]
fn answers_for_any_integer_negatives_included() {
    let mut vector = SparseVector::new();
    vector.set(-3, 3.0);
    assert_eq!(vector.get(-3), 3.0);
    assert_eq!(vector.get(3), 0.0);
    assert_eq!(vector.get(0), 0.0);
    // TypeScript stops at Number.MAX_SAFE_INTEGER (2^53-1); i64 goes further.
    assert_eq!(vector.get(i64::MAX), 0.0);
    assert_eq!(vector.get(i64::MIN), 0.0);
}

#[test]
fn stores_values_of_any_type() {
    #[derive(Clone, Debug, PartialEq)]
    enum Value {
        Object(i32),
        Flag(bool),
        Text(String),
    }

    let mut vector: SparseVector<Value> = SparseVector::with_default(Value::Object(0));
    vector.set(0, Value::Object(1));
    vector.set(1, Value::Flag(false));
    vector.set(2, Value::Text("text".to_string()));

    assert_eq!(vector.get(0), Value::Object(1));
    assert_eq!(vector.get(1), Value::Flag(false));
    assert_eq!(vector.get(2), Value::Text("text".to_string()));
}

// The TypeScript block's third case, `rejects a non-integer index`, needs no
// counterpart: an index is an i64, so 1.5, NaN and Infinity do not typecheck.
