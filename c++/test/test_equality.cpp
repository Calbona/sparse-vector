// Mirrors the `equality` block of the TypeScript suite.

#include <cmath>
#include <limits>
#include <string>
#include <vector>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

namespace {

/// Equality by length, standing in for a notion `operator==` cannot express.
bool by_length(const std::string& value, const std::string& defaultValue) {
  return value.size() == defaultValue.size();
}

/// Not transitive: one is within one of another, and of a third, without the first and third
/// having anything to do with each other.
bool within_one(const double& value, const double& defaultValue) {
  return std::abs(value - defaultValue) <= 1.0;
}

}  // namespace

SV_TEST(replaces_the_ordinary_comparison) {
  const std::string ab = "ab";
  sv::SV_vector<std::string> vector(ab);
  vector.set_equality(by_length);
  vector.set(1, "xy");  // a different string of the same length
  SV_CHECK_EQ(vector.get_element_amount(), 0u);
}

SV_TEST(prunes_what_the_new_predicate_calls_equal) {
  const std::string ab = "ab";
  sv::SV_vector<std::string> vector(ab);
  vector.set(1, "xy");
  vector.set(2, "x");
  SV_CHECK_EQ(vector.get_element_amount(), 2u);

  SV_CHECK_EQ(vector.set_equality(by_length), true);

  const std::vector<sv::SV_element<std::string>> expected{
      sv::SV_element<std::string>{2, "x"},
  };
  SV_CHECK_EQ(vector.elements(), expected);
}

SV_TEST(reports_the_predicate) {
  const std::string empty;
  sv::SV_vector<std::string> vector(empty);
  SV_CHECK(!vector.get_equality());

  vector.set_equality(by_length);
  SV_CHECK(static_cast<bool>(vector.get_equality()));
}

SV_TEST(restores_the_ordinary_comparison) {
  const std::string ab = "ab";
  sv::SV_vector<std::string> vector(ab);
  vector.set_equality(by_length);
  vector.set(1, "xy");
  SV_CHECK_EQ(vector.get_element_amount(), 0u);

  vector.set_equality(nullptr);
  vector.set(1, "xy");
  SV_CHECK_EQ(vector.get_element_amount(), 1u);
}

SV_TEST(travels_with_a_copy) {
  const std::string ab = "ab";
  sv::SV_vector<std::string> vector(ab);
  vector.set_equality(by_length);

  sv::SV_vector<std::string> copy = vector;
  SV_CHECK(static_cast<bool>(copy.get_equality()));
  copy.set(1, "xy");
  SV_CHECK_EQ(copy.get_element_amount(), 0u);
}

SV_TEST(compares_the_two_default_values) {
  const sv::SV_vector<double> a(0.0);
  const sv::SV_vector<double> other(5.0);
  const sv::SV_vector<double> same(0.0);
  SV_CHECK(!a.is_equal_to(other));
  SV_CHECK(a.is_equal_to(same));
}

SV_TEST(compares_every_element) {
  sv::SV_vector<double> a(0.0);
  sv::SV_vector<double> b(0.0);
  a.set(1, 7.0);
  b.set(1, 7.0);
  SV_CHECK(a.is_equal_to(b));

  b.set(2, 7.0);
  SV_CHECK(!a.is_equal_to(b));
}

SV_TEST(treats_a_missing_position_as_the_default) {
  const sv::SV_vector<double> a(0.0);
  sv::SV_vector<double> b(0.0);
  b.set(3, 9.0);

  SV_CHECK(!a.is_equal_to(b));
  const std::vector<sv::SV_element<double>> expected{sv::SV_element<double>{3, 0.0}};
  SV_CHECK_EQ(a.differences(b), expected);
}

SV_TEST(compares_across_different_default_values) {
  sv::SV_vector<double> a(0.0);
  sv::SV_vector<double> b(1.0);
  a.set_equality(within_one);
  b.set_equality(within_one);
  b.set(4, -1.0);  // further than one from b's default, so it stays; within one of a's

  SV_CHECK_EQ(a.get_element_amount(), 0u);
  SV_CHECK_EQ(b.get_element_amount(), 1u);
  SV_CHECK(a.is_equal_to(b));
  SV_CHECK(a.differences(b).empty());
}

SV_TEST(lists_what_differs_descending) {
  sv::SV_vector<double> a(0.0);
  a.set(1, 10.0);
  a.set(3, 30.0);
  a.set(5, 50.0);
  sv::SV_vector<double> b(0.0);
  b.set(3, 30.0);  // the one they agree on

  const std::vector<sv::SV_element<double>> expected{
      sv::SV_element<double>{5, 50.0},
      sv::SV_element<double>{1, 10.0},
  };
  SV_CHECK_EQ(a.differences(b), expected);
  SV_CHECK(!a.is_equal_to(b));
}

SV_TEST(carries_the_default_value_for_a_position_only_the_other_stores) {
  const sv::SV_vector<double> a(0.0);
  sv::SV_vector<double> b(0.0);
  b.set(2, 7.0);

  const std::vector<sv::SV_element<double>> expected{sv::SV_element<double>{2, 0.0}};
  SV_CHECK_EQ(a.differences(b), expected);
}

SV_TEST(refuses_to_list_differences_against_an_unequal_default) {
  const sv::SV_vector<double> a(0.0);
  const sv::SV_vector<double> b(5.0);
  SV_CHECK_THROWS(a.differences(b), std::invalid_argument);
}

SV_TEST(documents_the_nan_caveat_for_whole_vector_comparison) {
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const sv::SV_vector<double> empty_nan(nan);
  SV_CHECK(!empty_nan.is_equal_to(empty_nan));  // NaN is not equal to itself

  sv::SV_vector<double> stored(0.0);
  stored.set(1, nan);  // kept, since NaN != NaN
  SV_CHECK(!stored.is_equal_to(stored));

  const std::vector<sv::SV_element<double>> differing = stored.differences(stored);
  SV_CHECK_EQ(differing.size(), 1u);
  SV_CHECK_EQ(differing[0].index, 1);
  SV_CHECK(std::isnan(differing[0].value));
}

SV_TEST(refuses_to_list_differences_against_a_nan_default) {
  const double nan = std::numeric_limits<double>::quiet_NaN();
  const sv::SV_vector<double> vector(nan);
  SV_CHECK_THROWS(vector.differences(vector), std::invalid_argument);
}
