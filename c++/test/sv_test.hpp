#ifndef SV_TEST_HPP
#define SV_TEST_HPP

// A zero-dependency test harness: no framework, nothing to download, matching the
// habit of a library that has no dependencies of its own.
//
// Test-only. The library target exports `include/` and nothing else, so this
// header never reaches a consumer.

#include <cstddef>
#include <functional>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <sv/SV_element.hpp>

namespace sv_test {
namespace detail {

/// Whether `std::ostream << value` compiles.
template <class T, class = void>
struct is_streamable : std::false_type {};

template <class T>
struct is_streamable<T,
                     std::void_t<decltype(std::declval<std::ostream&>() << std::declval<const T&>())>>
    : std::true_type {};

}  // namespace detail
}  // namespace sv_test

namespace sv {

/// Test-support printer, so that a failed check on an element says which one.
///
/// Found by argument-dependent lookup, which is why it lives here rather than in
/// namespace sv_test.
template <class T>
std::enable_if_t<sv_test::detail::is_streamable<T>::value, std::ostream&> operator<<(
    std::ostream& out, const SV_element<T>& element) {
  return out << "{index: " << element.index << ", value: " << element.value << "}";
}

}  // namespace sv

namespace sv_test {

/// Thrown by a failing check, caught by run_all().
class Failure {
 public:
  explicit Failure(std::string message) : _message(std::move(message)) {}

  const std::string& message() const { return _message; }

 private:
  std::string _message;
};

/// A printable rendering of a value, for failure messages.
///
/// Falls back to a placeholder rather than refusing to compile, so that a check
/// on a type with no operator<< still works.
template <class T>
std::string describe(const T& value) {
  if constexpr (detail::is_streamable<T>::value) {
    std::ostringstream out;
    out << value;
    return out.str();
  } else {
    return "<unprintable>";
  }
}

inline std::string describe(bool value) { return value ? "true" : "false"; }

inline std::string describe(std::nullopt_t) { return "nullopt"; }

template <class T>
std::string describe(const std::optional<T>& value) {
  return value.has_value() ? describe(*value) : std::string("nullopt");
}

template <class T>
std::string describe(const std::vector<T>& values) {
  std::string out = "[";
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (i != 0) {
      out += ", ";
    }
    out += describe(values[i]);
  }
  out += "]";
  return out;
}

struct Case {
  std::string name;
  std::function<void()> fn;
};

/// The one registry, shared by every translation unit.
///
/// A function-local static rather than a namespace-scope global, because the test
/// files register themselves before main runs and this is immune to the static
/// initialization order fiasco.
inline std::vector<Case>& registry() {
  static std::vector<Case> cases;
  return cases;
}

struct Registrar {
  Registrar(const char* name, std::function<void()> fn) {
    registry().push_back(Case{name, std::move(fn)});
  }
};

/// Runs every registered case, returning a process exit code.
inline int run_all() {
  const std::vector<Case>& cases = registry();
  std::size_t failed = 0;

  for (const Case& test : cases) {
    try {
      test.fn();
      std::cout << "ok   " << test.name << '\n';
    } catch (const Failure& failure) {
      ++failed;
      std::cout << "FAIL " << test.name << "\n       " << failure.message() << '\n';
    } catch (const std::exception& error) {
      ++failed;
      std::cout << "FAIL " << test.name << "\n       unexpected exception: " << error.what()
                << '\n';
    }
  }

  std::cout << '\n' << (cases.size() - failed) << '/' << cases.size() << " passed\n";
  return failed == 0 ? 0 : 1;
}

}  // namespace sv_test

/// Declares a test case. The name is the TypeScript case name in snake_case, so
/// the three suites can be diffed against each other.
#define SV_TEST(name)                                                              \
  static void sv_test_case_##name();                                               \
  [[maybe_unused]] static const ::sv_test::Registrar sv_test_registrar_##name(     \
      #name, sv_test_case_##name);                                                 \
  static void sv_test_case_##name()

#define SV_FAIL(message) throw ::sv_test::Failure(message)

#define SV_CHECK(condition)                                    \
  do {                                                         \
    if (!(condition)) {                                        \
      SV_FAIL(std::string("SV_CHECK(" #condition ") failed")); \
    }                                                          \
  } while (false)

#define SV_CHECK_EQ(actual, expected)                                     \
  do {                                                                    \
    const auto& sv_actual = (actual);                                     \
    const auto& sv_expected = (expected);                                 \
    if (!(sv_actual == sv_expected)) {                                    \
      std::ostringstream sv_message;                                      \
      sv_message << "SV_CHECK_EQ(" #actual ", " #expected ") failed: "    \
                 << ::sv_test::describe(sv_actual) << " != "              \
                 << ::sv_test::describe(sv_expected);                     \
      SV_FAIL(sv_message.str());                                          \
    }                                                                     \
  } while (false)

#endif  // SV_TEST_HPP
