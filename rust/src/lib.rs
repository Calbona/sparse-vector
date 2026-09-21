//! The crate documentation lives in `README.md`, so the two cannot drift apart:
//! every ```rust block below is compiled and run by `cargo test`.
#![doc = include_str!("../README.md")]
#![forbid(unsafe_code)]
#![deny(missing_docs)]

mod element;
mod vector;

pub use element::Element;
pub use vector::{Iter, SparseVector};
