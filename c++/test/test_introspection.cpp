// Mirrors the `introspection` block of the TypeScript suite.

#include <cstddef>
#include <limits>
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
