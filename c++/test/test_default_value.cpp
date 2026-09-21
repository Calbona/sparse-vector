// Mirrors the `default value` block of the TypeScript suite.

#include <optional>
#include <string>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

SV_TEST(defaults_to_the_number_0_when_omitted) {
  const sv::SV_vector<> vector;
  SV_CHECK_EQ(vector.default_value(), 0.0);
  SV_CHECK_EQ(vector.get(5), 0.0);
}

SV_TEST(accepts_an_explicit_default) {
  const std::string empty;
  const sv::SV_vector<std::string> vector(empty);
  SV_CHECK_EQ(vector.get(5), std::string(""));
  SV_CHECK_EQ(vector.get(-5), std::string(""));
}

// Stands in for the TypeScript case `defaults to undefined when asked for it`:
// `undefined` is a first-class value there, and an optional is the nearest
// honest spelling of "the default is explicitly nothing".
SV_TEST(honours_an_explicit_empty_optional_default) {
  const sv::SV_vector<std::optional<int>> vector{std::optional<int>{}};
  SV_CHECK_EQ(vector.default_value(), std::nullopt);
  SV_CHECK_EQ(vector.get(0), std::nullopt);
}

SV_TEST(is_replaceable_after_construction) {
  sv::SV_vector<> vector(0.0);
  vector.set(1, 42.0);
  SV_CHECK_EQ(vector.get(2), 0.0);

  vector.set_default_value(-1.0);
  SV_CHECK_EQ(vector.get(2), -1.0);
  SV_CHECK_EQ(vector.get(1), 42.0);
}

SV_TEST(generalises_to_any_defaultable_type) {
  const sv::SV_vector<std::string> vector;
  SV_CHECK_EQ(vector.default_value(), std::string(""));
  SV_CHECK_EQ(vector.get(5), std::string(""));
}
