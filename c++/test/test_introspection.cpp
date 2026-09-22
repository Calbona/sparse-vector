// Mirrors the `introspection` block of the TypeScript suite.

#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

SV_TEST(lists_elements_in_ascending_index_order) {
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
  const std::vector<sv::index_type> expected_keys{-2, 0, 5};
  const std::vector<std::string> expected_values{"b", "c", "e"};

  SV_CHECK_EQ(vector.elements(), expected);
  SV_CHECK_EQ(vector.keys(), expected_keys);
  SV_CHECK_EQ(vector.values(), expected_values);
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

  const std::vector<sv::index_type> expected{1, 2};
  SV_CHECK_EQ(seen, expected);
}

SV_TEST(serialises_to_its_explicit_entries_only) {
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

  // The copy constructor is clone(); there is no member of that name.
  sv::SV_vector<std::string> copy = vector;
  copy.set(2, "b");
  copy.set_default_value("nine");

  const std::vector<sv::SV_element<std::string>> expected{
      sv::SV_element<std::string>{1, "a"},
  };
  SV_CHECK_EQ(vector.elements(), expected);
  SV_CHECK_EQ(vector.default_value(), std::string(""));
  SV_CHECK_EQ(copy.size(), 2u);
  SV_CHECK_EQ(copy.default_value(), std::string("nine"));
}

// No TypeScript counterpart. Fails the day someone swaps the storage for an
// unordered_map, because ascending order is a documented guarantee.
SV_TEST(keeps_ascending_order_under_adversarial_insertion) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  for (sv::index_type index = 49; index >= 0; --index) {
    vector.set(index, "x");
  }

  const std::vector<sv::index_type> keys = vector.keys();
  SV_CHECK_EQ(keys.size(), 50u);
  for (std::size_t i = 1; i < keys.size(); ++i) {
    SV_CHECK(keys[i - 1] < keys[i]);
  }
  SV_CHECK_EQ(keys.front(), sv::index_type{0});
  SV_CHECK_EQ(keys.back(), sv::index_type{49});
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
  SV_CHECK_EQ(vector.size(), 2u);
}

SV_TEST(reaches_an_entry_by_ordinal) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(-2, "b");
  vector.set(0, "c");
  vector.set(5, "e");

  // Ordinals number the entries, not the positions: 0 is the leftmost stored
  // entry however far out its index lies.
  const sv::SV_element<std::string> first{-2, "b"};
  const sv::SV_element<std::string> third{5, "e"};
  SV_CHECK_EQ(vector.element(0), first);
  SV_CHECK_EQ(vector.element(2), third);
  SV_CHECK_EQ(vector.element_index(1), sv::index_type{0});
  SV_CHECK_EQ(vector.element_value(1), std::string("c"));
}

SV_TEST(counts_from_the_right_by_ordinal) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(-2, "b");
  vector.set(0, "c");
  vector.set(5, "e");

  const sv::SV_element<std::string> last{5, "e"};
  const sv::SV_element<std::string> first{-2, "b"};
  SV_CHECK_EQ(vector.last_element(0), last);
  SV_CHECK_EQ(vector.last_element(2), first);
  SV_CHECK_EQ(vector.last_element_index(1), sv::index_type{0});
  SV_CHECK_EQ(vector.last_element_value(1), std::string("c"));
}

SV_TEST(refuses_an_ordinal_without_a_matching_entry) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");

  // One entry stored, so ordinal 0 reaches it and ordinal 1 is already past the
  // end, wherever that entry's index happens to lie.
  SV_CHECK_THROWS(vector.element(1), std::out_of_range);
  SV_CHECK_THROWS(vector.element_index(1), std::out_of_range);
  SV_CHECK_THROWS(vector.element_value(1), std::out_of_range);
  SV_CHECK_THROWS(vector.last_element(1), std::out_of_range);
  SV_CHECK_THROWS(vector.last_element_index(1), std::out_of_range);
  SV_CHECK_THROWS(vector.last_element_value(1), std::out_of_range);

  const sv::SV_vector<std::string> none(empty);
  SV_CHECK_THROWS(none.element(0), std::out_of_range);
  SV_CHECK_THROWS(none.last_element(0), std::out_of_range);
}

SV_TEST(measures_significant_positions_from_the_first_entry) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(10, "a");
  vector.set(13, "d");

  SV_CHECK_EQ(vector.left_significant_value(0), std::string("a"));
  // Positions holding no entry count, the way the zeros inside a number count.
  SV_CHECK_EQ(vector.left_significant_value(2), empty);
  SV_CHECK_EQ(vector.left_significant_value(3), std::string("d"));
  // A negative offset walks off the left of that first entry, into the default.
  SV_CHECK_EQ(vector.left_significant_value(-1), empty);
}

SV_TEST(measures_significant_positions_from_the_last_entry) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(10, "a");
  vector.set(13, "d");

  SV_CHECK_EQ(vector.right_significant_value(0), std::string("d"));
  SV_CHECK_EQ(vector.right_significant_value(3), std::string("a"));
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
  // The only entry is the anchor, so a step to its right would leave int64.
  SV_CHECK_EQ(high.left_significant_value(0), 1.0);
  SV_CHECK_EQ(high.left_significant_value(-1), 0.0);
  SV_CHECK_THROWS(high.left_significant_value(1), std::out_of_range);

  sv::SV_vector<> low;
  low.set(lowest, 2.0);
  SV_CHECK_EQ(low.right_significant_value(0), 2.0);
  SV_CHECK_EQ(low.right_significant_value(-1), 0.0);
  SV_CHECK_THROWS(low.right_significant_value(1), std::out_of_range);
}
