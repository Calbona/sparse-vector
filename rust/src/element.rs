/// A single stored position: the serialization shape, so a round trip has to carry the
/// default value alongside.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub struct Element<T = f64> {
    /// The position.
    pub index: i64,
    /// The value stored there.
    pub value: T,
}
