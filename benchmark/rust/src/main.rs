//! Benchmark of the Rust implementation:
//!
//!     cargo run --release --manifest-path benchmark/rust/Cargo.toml
//!
//! Prints one line per operation, "<name>\t<median milliseconds>", after warming up and taking the
//! median of several rounds. The index sequence, the element count and the timing discipline are
//! the same in all three implementations, so the numbers can be read side by side.

use std::hint::black_box;
use std::sync::Arc;
use std::time::Instant;

use sparse_vector::{Element, SparseVector};

const SIZE: usize = 100_000;
const ROUNDS: usize = 21;

/// The pseudo-random index sequence, identical in all three implementations.
fn indexes(count: usize) -> Vec<i64> {
    let mut out = Vec::with_capacity(count);
    let mut state: u32 = 12345;
    for _ in 0..count {
        state = state.wrapping_mul(1103515245).wrapping_add(12345) & 0x7FFF_FFFF;
        out.push(i64::from(state % 2_000_000) - 1_000_000);
    }
    out
}

fn median(mut samples: Vec<f64>) -> f64 {
    samples.sort_by(|left, right| left.partial_cmp(right).expect("no NaN"));
    samples[samples.len() / 2]
}

fn time_it<I, P, R>(name: &str, prepare: P, run: R)
where
    P: Fn() -> I,
    R: Fn(I) -> f64,
{
    for _ in 0..3 {
        black_box(run(prepare()));
    }
    let mut samples = Vec::with_capacity(ROUNDS);
    for _ in 0..ROUNDS {
        let input = prepare();
        let start = Instant::now();
        black_box(run(input));
        samples.push(start.elapsed().as_secs_f64() * 1e3);
    }
    println!("{name}\t{:.3}", median(samples));
}

fn filled(indexes: &[i64]) -> SparseVector<f64> {
    let mut vector = SparseVector::new(0.0);
    for (i, &index) in indexes.iter().enumerate() {
        vector.set(index, (i + 1) as f64);
    }
    vector
}

fn sevens(indexes: &[i64]) -> SparseVector<f64> {
    let mut vector = SparseVector::new(0.0);
    for &index in indexes {
        vector.set(index, 7.0);
    }
    vector
}

fn main() {
    let indexes = indexes(SIZE);
    let elements: Vec<Element<f64>> = indexes
        .iter()
        .enumerate()
        .map(|(i, &index)| Element {
            index,
            value: (i + 1) as f64,
        })
        .collect();

    let filled_here = || filled(&indexes);
    let sevens_here = || sevens(&indexes);

    println!("stored\t{}", filled_here().get_element_amount());

    time_it("set", || (), |_| {
        let mut vector = SparseVector::new(0.0);
        for (i, &index) in indexes.iter().enumerate() {
            vector.set(index, (i + 1) as f64);
        }
        vector.get_element_amount() as f64
    });

    time_it("get", &filled_here, |vector| {
        let mut sum = 0.0;
        for &index in &indexes {
            sum += vector.get(index);
        }
        sum
    });

    time_it("from_elements", || elements.clone(), |elements| {
        SparseVector::from_elements(elements, 0.0).get_element_amount() as f64
    });

    time_it("iterate", &filled_here, |vector| {
        let mut sum = 0.0;
        for element in &vector {
            sum += element.value;
        }
        sum
    });

    time_it("replace default (full prune)", &sevens_here, |mut vector| {
        f64::from(vector.set_default_value(7.0))
    });

    time_it("replace predicate (full prune)", &filled_here, |mut vector| {
        f64::from(vector.set_equality(Some(Arc::new(|_: &f64, _: &f64| true))))
    });

    // The pair is built once and reused: rebuilding it every round would leave the heap churned
    // right before the timed region, and both operations are pure reads.
    let equal_pair = (filled_here(), filled_here());
    let mut differing_pair = (filled_here(), filled_here());
    differing_pair.1.set(indexes[SIZE - 1], 999_999_999.0);

    time_it("is_equal_to (equal)", || &equal_pair, |(a, b)| {
        f64::from(a.is_equal_to(b))
    });

    time_it("differences (one element)", || &differing_pair, |(a, b)| {
        a.differences(b).len() as f64
    });
}
