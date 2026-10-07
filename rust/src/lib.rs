//! The crate documentation lives in `README.md`, so the two cannot drift apart.
#![doc = include_str!("../README.md")]
#![forbid(unsafe_code)]
#![deny(missing_docs)]

mod element;
mod vector;

pub use element::Element;
pub use vector::{Equality, Iter, SparseVector};
