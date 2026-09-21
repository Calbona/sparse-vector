// Mirrors the `pruning of default-valued entries` block of the TypeScript suite.

#include <limits>
#include <optional>
#include <string>
#include <vector>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

namespace {

/// A user type whose equality is structural. At namespace scope because MSVC
/// rejects a friend function defined inside a local class (C2689).
struct Widget {
  int a;
  friend bool operator==(const Widget& x, const Widget& y) { return x.a == y.a; }
};

}  // namespace

SV_TEST(never_stores_a_value_equal_to_the_default) {
  sv::SV_vector<> vector;
  vector.set(1, 0.0);
  SV_CHECK_EQ(vector.size(), 0u);
  SV_CHECK_EQ(vector.has(1), false);
}

SV_TEST(prunes_an_entry_that_becomes_the_default) {
  sv::SV_vector<> vector(0.0);
  vector.set(1, 5.0);
  SV_CHECK_EQ(vector.size(), 1u);

  vector.set(1, 0.0);
  SV_CHECK_EQ(vector.size(), 0u);
  SV_CHECK_EQ(vector.get(1), 0.0);
}

SV_TEST(prunes_on_a_change_of_default_and_forgets_pruned_positions) {
  using Value = std::optional<int>;

  sv::SV_vector<Value> vector{Value{0}};
  vector.set(1, Value{});
  vector.set(2, Value{7});

  const std::vector<sv::SV_element<Value>> before{
      sv::SV_element<Value>{1, Value{}},
      sv::SV_element<Value>{2, Value{7}},
  };
  SV_CHECK_EQ(vector.elements(), before);

  vector.set_default_value(Value{});

  const std::vector<sv::SV_element<Value>> after{
      sv::SV_element<Value>{2, Value{7}},
  };
  SV_CHECK_EQ(vector.elements(), after);
  SV_CHECK_EQ(vector.get(3), Value{});
  SV_CHECK_EQ(vector.size(), 1u);
}

SV_TEST(keeps_entries_that_were_pruned_earlier_gone) {
  using Value = std::optional<int>;

  sv::SV_vector<Value> vector{Value{0}};
  vector.set(1, Value{0});
  vector.set(2, Value{7});
  SV_CHECK_EQ(vector.has(1), false);

  vector.set_default_value(Value{});
  vector.set_default_value(Value{0});

  const std::vector<sv::SV_element<Value>> expected{
      sv::SV_element<Value>{2, Value{7}},
  };
  SV_CHECK_EQ(vector.elements(), expected);
}

// The TypeScript case stores 0, '0', false and null, which are four distinct
// types there and cannot share a single T here. Only an exact match drops an
// entry, so the stand-in needs values that differ while looking alike.
SV_TEST(compares_exactly) {
  const std::string zero = "0";
  sv::SV_vector<std::string> vector(zero);
  vector.set(1, "00");
  vector.set(2, "0 ");
  vector.set(3, "");
  SV_CHECK_EQ(vector.size(), 3u);
}

SV_TEST(documents_the_nan_caveat) {
  // NaN != NaN, so a NaN entry is never pruned by a NaN default. Same behaviour
  // as the TypeScript implementation.
  sv::SV_vector<> vector(std::numeric_limits<double>::quiet_NaN());
  vector.set(1, std::numeric_limits<double>::quiet_NaN());
  SV_CHECK_EQ(vector.size(), 1u);
}

// No TypeScript counterpart, and a real divergence: `===` there compares object
// references, while operator== here is structural. The README gives the
// shared_ptr recipe when identity is what you want.
SV_TEST(prunes_by_value_not_by_identity) {
  sv::SV_vector<Widget> vector{Widget{0}};
  vector.set(1, Widget{0});
  SV_CHECK_EQ(vector.size(), 0u);

  vector.set(2, Widget{1});
  SV_CHECK_EQ(vector.size(), 1u);
}

SV_TEST(prunes_a_negative_zero) {
  // -0.0 == 0.0, and TypeScript's -0 === 0 does the same, so this is a
  // consistency check rather than a divergence.
  sv::SV_vector<> vector;
  vector.set(1, -0.0);
  SV_CHECK_EQ(vector.size(), 0u);
}
