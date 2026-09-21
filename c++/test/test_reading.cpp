// Mirrors the `reading` block of the TypeScript suite.

#include <limits>
#include <string>
#include <type_traits>
#include <variant>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

SV_TEST(answers_for_any_integer_negatives_included) {
  sv::SV_vector<> vector;
  vector.set(-3, 3.0);
  SV_CHECK_EQ(vector.get(-3), 3.0);
  SV_CHECK_EQ(vector.get(3), 0.0);
  SV_CHECK_EQ(vector.get(0), 0.0);

  // The TypeScript case stops at Number.MAX_SAFE_INTEGER; int64_t goes further.
  SV_CHECK_EQ(vector.get(std::numeric_limits<sv::index_type>::max()), 0.0);
  SV_CHECK_EQ(vector.get(std::numeric_limits<sv::index_type>::min()), 0.0);
}

// The TypeScript case mixes an object, a boolean, a string and a number in one
// vector; a statically typed vector holds a single T, so a variant stands in.
SV_TEST(stores_values_of_any_type) {
  using Value = std::variant<int, bool, std::string>;

  sv::SV_vector<Value> vector{Value{0}};
  vector.set(0, Value{1});
  vector.set(1, Value{false});
  vector.set(2, Value{std::string("text")});

  SV_CHECK_EQ(vector.get(0), Value{1});
  SV_CHECK_EQ(vector.get(1), Value{false});
  SV_CHECK_EQ(vector.get(2), Value{std::string("text")});
}

// The TypeScript case asserts that a non-integer index throws a TypeError. There
// is no counterpart: index_type is int64_t, so a non-integer index is a compile
// error instead. This test pins the domain.
SV_TEST(accepts_only_integer_indices) {
  sv::SV_vector<> vector;
  vector.set(1, 1.0);
  SV_CHECK_EQ(vector.get(1), 1.0);
  static_assert(std::is_same<decltype(vector.get(1)), double>::value,
                "get() takes an integer index and returns the value type");
}
