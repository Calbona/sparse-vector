// Mirrors the `SV_vector.from` block of the TypeScript suite.

#include <string>
#include <vector>

#include <sv/SV_vector.hpp>

#include "sv_test.hpp"

SV_TEST(builds_from_elements_dropping_defaults_and_favouring_the_last_duplicate) {
  using Element = sv::SV_element<std::string>;

  const std::vector<Element> input{
      Element{3, "c"},
      Element{1, "a"},
      Element{3, "last"},
      Element{4, ""},  // equal to the default, so dropped
  };

  const std::string empty;
  const sv::SV_vector<std::string> vector = sv::SV_vector<std::string>::from(input, empty);

  const std::vector<Element> expected{Element{1, "a"}, Element{3, "last"}};
  SV_CHECK_EQ(vector.elements(), expected);
}

SV_TEST(takes_a_default_value) {
  using Element = sv::SV_element<std::string>;

  const std::vector<Element> input{Element{1, "x"}};
  const sv::SV_vector<std::string> vector = sv::SV_vector<std::string>::from(input, "z");

  SV_CHECK_EQ(vector.default_value(), std::string("z"));
  SV_CHECK_EQ(vector.get(99), std::string("z"));
}

// Stands in for the TypeScript case `round-trips through JSON when the default is
// carried alongside`. There is no JSON encoder here, so the round trip goes
// through elements(), which is the contract.
SV_TEST(round_trips_when_the_default_is_carried_alongside) {
  const std::string none = "none";
  sv::SV_vector<std::string> original(none);
  original.set(-1, "a");
  original.set(10, "b");

  const sv::SV_vector<std::string> restored =
      sv::SV_vector<std::string>::from(original.elements(), none);

  SV_CHECK_EQ(restored.elements(), original.elements());
  SV_CHECK_EQ(restored.get(0), std::string("none"));
}

// The TypeScript signature takes any Iterable; this takes any input iterator.
SV_TEST(accepts_any_input_iterator) {
  using Element = sv::SV_element<sv::index_type>;

  const std::vector<Element> input{
      Element{0, 0},
      Element{1, 2},
      Element{2, 4},
      Element{3, 6},
  };

  const sv::SV_vector<sv::index_type> vector =
      sv::SV_vector<sv::index_type>::from_elements(input.begin(), input.end(), 0);

  SV_CHECK_EQ(vector.size(), 3u);  // index 0 holds 0, which is the default

  const std::vector<sv::index_type> expected_keys{1, 2, 3};
  SV_CHECK_EQ(vector.keys(), expected_keys);
}
