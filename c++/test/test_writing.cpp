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
  SV_CHECK_EQ(vector.size(), 1u);

  vector.set(1, "b");
  SV_CHECK_EQ(vector.get(1), std::string("b"));
  SV_CHECK_EQ(vector.size(), 1u);
}

SV_TEST(deletes_to_the_default_value) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a");

  SV_CHECK_EQ(vector.erase(1), true);
  SV_CHECK_EQ(vector.get(1), std::string(""));
  SV_CHECK_EQ(vector.size(), 0u);
  SV_CHECK_EQ(vector.erase(1), false);
}

SV_TEST(clears_every_entry_but_keeps_the_default) {
  const std::string gone = "gone";
  sv::SV_vector<std::string> vector(gone);
  vector.set(1, "a");
  vector.set(-2, "b");

  vector.clear();
  SV_CHECK_EQ(vector.size(), 0u);
  SV_CHECK_EQ(vector.default_value(), std::string("gone"));
  SV_CHECK_EQ(vector.get(1), std::string("gone"));
}

SV_TEST(chains_writes) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  vector.set(1, "a").set(2, "b").set(3, "c");

  const std::vector<sv::index_type> expected{1, 2, 3};
  SV_CHECK_EQ(vector.keys(), expected);
}
