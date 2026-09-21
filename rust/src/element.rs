/// A single stored position of a [`SparseVector`](crate::SparseVector).
///
/// This is the serialization shape: a vector serializes to its elements only, so
/// a round trip has to carry the default value alongside.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub struct Element<T = f64> {
    /// The position, negative or not.
    pub index: i64,
    /// The value stored there.
    pub value: T,
}
