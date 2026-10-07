// Mirrors the `writing` block of the TypeScript suite.

#include <string>
#include <vector>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

SV_TEST(inserts_and_updates) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");
  SV_CHECK_EQ(vector.get(1), std::string("a"));
  SV_CHECK_EQ(vector.get_element_amount(), 1u);

  vector.set(1, "b");
  SV_CHECK_EQ(vector.get(1), std::string("b"));
  SV_CHECK_EQ(vector.get_element_amount(), 1u);
}

SV_TEST(resets_to_the_default_value) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");

  SV_CHECK_EQ(vector.reset_value(1), true);
  SV_CHECK_EQ(vector.get(1), std::string(""));
  SV_CHECK_EQ(vector.get_element_amount(), 0u);
  SV_CHECK_EQ(vector.reset_value(1), false);
}

SV_TEST(resets_every_element_but_keeps_the_default) {
  const std::string gone = "gone";
  sv::SV_vector<std::string> vector(gone);
  SV_CHECK_EQ(vector.reset_vector(), false);
  vector.set(1, "a");
  vector.set(-2, "b");

  SV_CHECK_EQ(vector.reset_vector(), true);
  SV_CHECK_EQ(vector.reset_vector(), false);
  SV_CHECK_EQ(vector.get_element_amount(), 0u);
  SV_CHECK_EQ(vector.get_default_value(), std::string("gone"));
  SV_CHECK_EQ(vector.get(1), std::string("gone"));
}

SV_TEST(chains_writes) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a").set(2, "b").set(3, "c");

  const std::vector<sv::index_type> expected{3, 2, 1};
  SV_CHECK_EQ(vector.indexes(), expected);
}
