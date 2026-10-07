// Mirrors the `introspection` block of the TypeScript suite.

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

SV_TEST(lists_elements_in_descending_index_order) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(5, "e");
  vector.set(-2, "b");
  vector.set(0, "c");

  const std::vector<sv::SV_element<std::string>> expected{
      sv::SV_element<std::string>{5, "e"},
      sv::SV_element<std::string>{0, "c"},
      sv::SV_element<std::string>{-2, "b"},
  };
  const std::vector<sv::index_type> expected_indexes{5, 0, -2};
  const std::vector<std::string> expected_values{"e", "c", "b"};

  SV_CHECK_EQ(vector.elements(), expected);
  SV_CHECK_EQ(vector.indexes(), expected_indexes);
  SV_CHECK_EQ(vector.values(), expected_values);
}

SV_TEST(lists_the_inverted_views_in_ascending_index_order) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(5, "e");
  vector.set(-2, "b");
  vector.set(0, "c");

  const std::vector<sv::SV_element<std::string>> expected{
      sv::SV_element<std::string>{-2, "b"},
      sv::SV_element<std::string>{0, "c"},
      sv::SV_element<std::string>{5, "e"},
  };
  const std::vector<sv::index_type> expected_indexes{-2, 0, 5};
  const std::vector<std::string> expected_values{"b", "c", "e"};

  SV_CHECK_EQ(vector.inverted_elements(), expected);
  SV_CHECK_EQ(vector.inverted_indexes(), expected_indexes);
  SV_CHECK_EQ(vector.inverted_values(), expected_values);

  // The ordinal trio numbers the inverted listing: inverted_element(n) is
  // inverted_elements()[n], read from its first element. Named locals because
  // SV_CHECK_EQ binds a reference to each side, which a temporary cannot survive.
  const std::vector<sv::SV_element<std::string>> listed = vector.inverted_elements();
  const std::vector<sv::index_type> listed_indexes = vector.inverted_indexes();
  SV_CHECK_EQ(listed_indexes.front(), listed.front().index);
  SV_CHECK_EQ(vector.inverted_element(1), listed[1]);
}

SV_TEST(reports_the_element_count) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  SV_CHECK_EQ(vector.get_element_amount(), 0u);

  vector.set(1, "a");
  SV_CHECK_EQ(vector.get_element_amount(), 1u);

  vector.set(1, "");  // equal to the default, so dropped again
  SV_CHECK_EQ(vector.get_element_amount(), 0u);
}

SV_TEST(measures_the_span_between_the_outermost_elements) {
  sv::SV_vector<std::string> vector(std::string(""));
  // Nothing stored spans nothing.
  SV_CHECK_EQ(vector.get_significant_dimension(), 0);

  vector.set(-3, "a");
  // A single element spans itself.
  SV_CHECK_EQ(vector.get_significant_dimension(), 1);

  // -2 through 5 inclusive: the positions in between count, so this is not the
  // element count.
  sv::SV_vector<std::string> span(std::string(""));
  span.set(5, "e");
  span.set(-2, "b");
  SV_CHECK_EQ(span.get_significant_dimension(), 8);
}

SV_TEST(measures_how_far_the_vector_reaches_each_side_of_zero) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  // Nothing stored reaches nowhere.
  SV_CHECK_EQ(vector.get_plus_dimension(), 0);
  SV_CHECK_EQ(vector.get_minus_dimension(), 0);

  // Entries on one side only leave the other side at zero.
  sv::SV_vector<std::string> positive(empty);
  positive.set(3, "a");
  positive.set(5, "b");
  SV_CHECK_EQ(positive.get_plus_dimension(), 5);
  SV_CHECK_EQ(positive.get_minus_dimension(), 0);

  sv::SV_vector<std::string> negative(empty);
  negative.set(-3, "a");
  negative.set(-5, "b");
  SV_CHECK_EQ(negative.get_plus_dimension(), 0);
  SV_CHECK_EQ(negative.get_minus_dimension(), 5);

  // Straddling zero: 5 above, 2 below, and 8 positions from end to end.
  sv::SV_vector<std::string> span(empty);
  span.set(5, "e");
  span.set(-2, "b");
  SV_CHECK_EQ(span.get_plus_dimension(), 5);
  SV_CHECK_EQ(span.get_minus_dimension(), 2);
  SV_CHECK_EQ(span.get_significant_dimension(), 8);
}

SV_TEST(is_iterable) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(2, "b");
  vector.set(1, "a");

  std::vector<sv::index_type> seen;
  for (const auto& [index, value] : vector) {
    (void)value;
    seen.push_back(index);
  }

  const std::vector<sv::index_type> expected{2, 1};
  SV_CHECK_EQ(seen, expected);
}

SV_TEST(serialises_to_its_explicit_elements_only) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");
  vector.set(2, "");  // equal to the default, so never stored

  const std::vector<sv::SV_element<std::string>> expected{
      sv::SV_element<std::string>{1, "a"},
  };
  SV_CHECK_EQ(vector.elements(), expected);
}

SV_TEST(clones_independently) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");

  // Read before copying. The C++ container answers ordered reads out of an index it keeps, and
  // that index points into the storage it was built from, so a copy must build its own rather
  // than inherit one. Writing over the original right after is what would expose a shared index:
  // it would hand the copy the original's new value.
  (void)vector.indexes();

  // The copy constructor is clone(); there is no member of that name.
  sv::SV_vector<std::string> copy = vector;
  vector.set(1, "changed");
  const std::vector<std::string> mine{"a"};
  SV_CHECK_EQ(copy.values(), mine);

  copy.set(2, "b");
  copy.set_default_value("nine");

  const std::vector<sv::SV_element<std::string>> expected{
      sv::SV_element<std::string>{2, "b"},
      sv::SV_element<std::string>{1, "a"},
  };
  SV_CHECK_EQ(copy.elements(), expected);
  SV_CHECK_EQ(vector.get_default_value(), std::string(""));
  SV_CHECK_EQ(copy.get_element_amount(), 2u);
  SV_CHECK_EQ(copy.get_default_value(), std::string("nine"));

  const std::vector<sv::index_type> both{2, 1};
  SV_CHECK_EQ(copy.indexes(), both);

  const std::vector<std::string> theirs{"changed"};
  SV_CHECK_EQ(vector.values(), theirs);
}

// No TypeScript counterpart: it pins the ordered index the C++ container keeps on top of its hash
// map. The index is dropped when the set of stored indices changes and kept when a value is merely
// overwritten, so both paths have to read back fresh.
SV_TEST(reflects_a_write_made_after_a_read) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");
  vector.set(3, "c");
  (void)vector.indexes();  // builds the ordered index

  vector.set(5, "e");  // a new index, so the order has to change
  const std::vector<sv::index_type> descending{5, 3, 1};
  SV_CHECK_EQ(vector.indexes(), descending);
  SV_CHECK_EQ(vector.element(0), (sv::SV_element<std::string>{5, "e"}));

  vector.set(3, "changed");  // an existing index, so the order stands but the value must not
  SV_CHECK_EQ(vector.indexes(), descending);
  const std::vector<std::string> values{"e", "changed", "a"};
  SV_CHECK_EQ(vector.values(), values);

  SV_CHECK(vector.reset_value(3));
  const std::vector<sv::index_type> shortened{5, 1};
  SV_CHECK_EQ(vector.indexes(), shortened);

  SV_CHECK(vector.reset_vector());
  SV_CHECK_EQ(vector.indexes().size(), 0u);
  SV_CHECK_EQ(vector.get_default_value(), std::string(""));
}

// No TypeScript counterpart: neither TypeScript nor Rust has a move.
SV_TEST(moves_without_carrying_a_stale_order) {
  const std::string empty;
  sv::SV_vector<std::string> source(empty);
  source.set(4, "d");
  source.set(-2, "b");
  (void)source.indexes();

  sv::SV_vector<std::string> moved = std::move(source);
  const std::vector<sv::index_type> descending{4, -2};
  SV_CHECK_EQ(moved.indexes(), descending);
  SV_CHECK_EQ(moved.element_value(0), std::string("d"));

  sv::SV_vector<std::string> assigned(empty);
  assigned.set(9, "i");
  (void)assigned.indexes();
  assigned = moved;
  SV_CHECK_EQ(assigned.indexes(), descending);
  SV_CHECK_EQ(assigned.get_element_amount(), 2u);
}

// No TypeScript counterpart. Fails the day someone swaps the storage for an
// unordered_map, because descending order is a documented guarantee.
SV_TEST(keeps_descending_order_under_adversarial_insertion) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  for (sv::index_type index = 0; index < 50; ++index) {
    vector.set(index, "x");
  }

  const std::vector<sv::index_type> indexes = vector.indexes();
  SV_CHECK_EQ(indexes.size(), 50u);
  for (std::size_t i = 1; i < indexes.size(); ++i) {
    SV_CHECK(indexes[i - 1] > indexes[i]);
  }
  SV_CHECK_EQ(indexes.front(), sv::index_type{49});
  SV_CHECK_EQ(indexes.back(), sv::index_type{0});
}

// No TypeScript counterpart: its indices are doubles capped at 2^53-1.
SV_TEST(supports_the_whole_int64_index_range) {
  constexpr sv::index_type lowest = std::numeric_limits<sv::index_type>::min();
  constexpr sv::index_type highest = std::numeric_limits<sv::index_type>::max();

  sv::SV_vector<> vector;
  vector.set(lowest, 1.0);
  vector.set(highest, 2.0);

  SV_CHECK_EQ(vector.get(lowest), 1.0);
  SV_CHECK_EQ(vector.get(highest), 2.0);
  SV_CHECK_EQ(vector.get_element_amount(), 2u);
}

SV_TEST(reaches_an_element_by_ordinal) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(-2, "b");
  vector.set(0, "c");
  vector.set(5, "e");

  // Ordinals number the elements, not the positions: 0 is the leftmost stored
  // element however far out its index lies.
  const sv::SV_element<std::string> first{5, "e"};
  const sv::SV_element<std::string> third{-2, "b"};
  SV_CHECK_EQ(vector.element(0), first);
  SV_CHECK_EQ(vector.element(2), third);
  SV_CHECK_EQ(vector.element_index(1), sv::index_type{0});
  SV_CHECK_EQ(vector.element_value(1), std::string("c"));
}

SV_TEST(counts_from_the_inverted_end_by_ordinal) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(-2, "b");
  vector.set(0, "c");
  vector.set(5, "e");

  const sv::SV_element<std::string> rightmost{-2, "b"};
  const sv::SV_element<std::string> leftmost{5, "e"};
  SV_CHECK_EQ(vector.inverted_element(0), rightmost);
  SV_CHECK_EQ(vector.inverted_element(2), leftmost);
  SV_CHECK_EQ(vector.inverted_element_index(1), sv::index_type{0});
  SV_CHECK_EQ(vector.inverted_element_value(1), std::string("c"));
}

SV_TEST(refuses_an_ordinal_without_a_matching_element) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");

  // One element stored, so ordinal 0 reaches it and ordinal 1 is past the end.
  SV_CHECK_THROWS(vector.element(1), std::out_of_range);
  SV_CHECK_THROWS(vector.element_index(1), std::out_of_range);
  SV_CHECK_THROWS(vector.element_value(1), std::out_of_range);
  SV_CHECK_THROWS(vector.inverted_element(1), std::out_of_range);
  SV_CHECK_THROWS(vector.inverted_element_index(1), std::out_of_range);
  SV_CHECK_THROWS(vector.inverted_element_value(1), std::out_of_range);

  const sv::SV_vector<std::string> none(empty);
  SV_CHECK_THROWS(none.element(0), std::out_of_range);
  SV_CHECK_THROWS(none.inverted_element(0), std::out_of_range);
}

SV_TEST(measures_significant_positions_from_the_first_element) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(10, "a");
  vector.set(13, "d");

  // The leftmost element is the most significant digit.
  SV_CHECK_EQ(vector.left_significant_value(0), std::string("d"));
  // Positions holding no element count, like the zeros inside a number.
  SV_CHECK_EQ(vector.left_significant_value(2), empty);
  SV_CHECK_EQ(vector.left_significant_value(3), std::string("a"));
  // A negative offset walks off the left of that first element, into the default.
  SV_CHECK_EQ(vector.left_significant_value(-1), empty);
}

SV_TEST(measures_significant_positions_from_the_last_element) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(10, "a");
  vector.set(13, "d");

  SV_CHECK_EQ(vector.right_significant_value(0), std::string("a"));
  SV_CHECK_EQ(vector.right_significant_value(3), std::string("d"));
  SV_CHECK_EQ(vector.right_significant_value(4), empty);
  SV_CHECK_EQ(vector.right_significant_value(-1), empty);
}

SV_TEST(refuses_a_significant_position_without_an_anchor) {
  const std::string empty;
  const sv::SV_vector<std::string> none(empty);

  SV_CHECK_THROWS(none.left_significant_value(0), std::out_of_range);
  SV_CHECK_THROWS(none.right_significant_value(0), std::out_of_range);
}

// No TypeScript counterpart: its indices are doubles, so a step past
// Number.MAX_SAFE_INTEGER loses precision rather than leaving the range.
SV_TEST(handles_a_position_at_the_edge_of_the_index_range) {
  constexpr sv::index_type lowest = std::numeric_limits<sv::index_type>::min();
  constexpr sv::index_type highest = std::numeric_limits<sv::index_type>::max();

  sv::SV_vector<> high;
  high.set(highest, 1.0);
  // One element, anchoring both methods: one step below it is reachable from
  // either end, one step above it is not.
  SV_CHECK_EQ(high.left_significant_value(0), 1.0);
  SV_CHECK_EQ(high.left_significant_value(1), 0.0);
  SV_CHECK_EQ(high.right_significant_value(-1), 0.0);
  SV_CHECK_THROWS(high.left_significant_value(-1), std::out_of_range);
  SV_CHECK_THROWS(high.right_significant_value(1), std::out_of_range);

  sv::SV_vector<> low;
  low.set(lowest, 2.0);
  SV_CHECK_EQ(low.right_significant_value(0), 2.0);
  SV_CHECK_EQ(low.right_significant_value(1), 0.0);
  SV_CHECK_EQ(low.left_significant_value(-1), 0.0);
  SV_CHECK_THROWS(low.right_significant_value(-1), std::out_of_range);
  SV_CHECK_THROWS(low.left_significant_value(1), std::out_of_range);
}
