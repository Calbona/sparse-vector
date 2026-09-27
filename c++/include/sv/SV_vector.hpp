#ifndef SV_VECTOR_HPP
#define SV_VECTOR_HPP

#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "SV_element.hpp"

namespace sv {

/// A sparse vector: a mapping from integer index to arbitrary value that stores only the
/// positions differing from the default value. Entries are ordered largest index leftmost.
template <class T = double>
class SV_vector {
 public:
  using index_type = sv::index_type;
  using value_type = T;
  using element_type = SV_element<T>;
  /// The entry container, ordered by descending index, so begin() is the leftmost entry.
  using container_type = std::map<index_type, T, std::greater<index_type>>;

  /// A vector whose empty positions read back as `T{}`.
  SV_vector() = default;

  /// A vector whose empty positions read back as `defaultValue`.
  explicit SV_vector(const T& defaultValue) : _default(defaultValue) {}

  /// @copydoc SV_vector(const T&)
  explicit SV_vector(T&& defaultValue) : _default(std::move(defaultValue)) {}

  SV_vector(const SV_vector&) = default;
  SV_vector(SV_vector&&) noexcept = default;
  SV_vector& operator=(const SV_vector&) = default;
  SV_vector& operator=(SV_vector&&) noexcept = default;
  ~SV_vector() = default;

  /// The value reported for every position with no explicit entry.
  ///
  /// There is deliberately no non-const overload; use set_default_value(), which prunes.
  [[nodiscard]] const T& get_default_value() const noexcept { return _default; }

  /// Replaces the default value, immediately pruning every entry equal to it. Pruning is
  /// one-way: the previous default is not remembered.
  void set_default_value(const T& next) {
    _default = next;
    _prune();
  }

  /// @copydoc set_default_value(const T&)
  void set_default_value(T&& next) {
    _default = std::move(next);
    _prune();
  }

  /// The number of explicitly stored entries.
  [[nodiscard]] std::size_t get_element_amount() const noexcept { return _entries.size(); }

  /// The distance between the leftmost and rightmost entries, both included; zero when empty.
  ///
  /// @throws std::out_of_range when the span leaves the index_type range.
  [[nodiscard]] index_type get_significant_dimension() const {
    if (_entries.empty()) {
      return 0;
    }
    const index_type leftmost = _entries.begin()->first;
    const index_type rightmost = std::prev(_entries.end())->first;
    return _checked_add(_checked_subtract(leftmost, rightmost), 1);
  }

  /// How far the vector reaches above zero: the leftmost index itself, or zero.
  [[nodiscard]] index_type get_plus_dimension() const noexcept {
    if (_entries.empty()) {
      return 0;
    }
    const index_type leftmost = _entries.begin()->first;
    return leftmost > 0 ? leftmost : 0;
  }

  /// How far the vector reaches below zero: the rightmost index negated, or zero.
  ///
  /// @throws std::out_of_range when the negation leaves the index_type range.
  [[nodiscard]] index_type get_minus_dimension() const {
    if (_entries.empty()) {
      return 0;
    }
    const index_type rightmost = std::prev(_entries.end())->first;
    return rightmost < 0 ? _checked_subtract(0, rightmost) : 0;
  }

  /// The value at `index`, or the default value when nothing is stored there. Reading is
  /// total, so it returns by value: the default branch has no entry to refer to.
  [[nodiscard]] T get(index_type index) const {
    const auto entry = _entries.find(index);
    if (entry == _entries.end()) {
      return _default;
    }
    return entry->second;
  }

  /// Inserts or updates `value` at `index`, returning `*this` so calls chain. Writing the
  /// default removes whatever was there instead of storing it.
  SV_vector& set(index_type index, const T& value) {
    if (value == _default) {
      _entries.erase(index);
    } else {
      _entries.insert_or_assign(index, value);
    }
    return *this;
  }

  /// @copydoc set(index_type, const T&)
  SV_vector& set(index_type index, T&& value) {
    if (value == _default) {
      _entries.erase(index);
    } else {
      _entries.insert_or_assign(index, std::move(value));
    }
    return *this;
  }

  /// Resets the entry at `index`, returning whether there was one; it then reads as the default.
  bool reset_value(index_type index) { return _entries.erase(index) != 0; }

  /// Resets every explicit entry, keeping the default value.
  void reset_vector() noexcept { _entries.clear(); }

  /// The explicit entries, descending by index.
  [[nodiscard]] std::vector<element_type> elements() const {
    std::vector<element_type> out;
    out.reserve(_entries.size());
    for (const auto& entry : _entries) {
      out.push_back(element_type{entry.first, entry.second});
    }
    return out;
  }

  /// The same entries, ascending by index.
  [[nodiscard]] std::vector<element_type> inverted_elements() const {
    std::vector<element_type> out;
    out.reserve(_entries.size());
    for (auto entry = _entries.rbegin(); entry != _entries.rend(); ++entry) {
      out.push_back(element_type{entry->first, entry->second});
    }
    return out;
  }

  /// The stored indices, descending.
  [[nodiscard]] std::vector<index_type> indexes() const {
    std::vector<index_type> out;
    out.reserve(_entries.size());
    for (const auto& entry : _entries) {
      out.push_back(entry.first);
    }
    return out;
  }

  /// The same indices, ascending.
  [[nodiscard]] std::vector<index_type> inverted_indexes() const {
    std::vector<index_type> out;
    out.reserve(_entries.size());
    for (auto entry = _entries.rbegin(); entry != _entries.rend(); ++entry) {
      out.push_back(entry->first);
    }
    return out;
  }

  /// The stored values, descending by index.
  [[nodiscard]] std::vector<T> values() const {
    std::vector<T> out;
    out.reserve(_entries.size());
    for (const auto& entry : _entries) {
      out.push_back(entry.second);
    }
    return out;
  }

  /// The same values, ascending by index.
  [[nodiscard]] std::vector<T> inverted_values() const {
    std::vector<T> out;
    out.reserve(_entries.size());
    for (auto entry = _entries.rbegin(); entry != _entries.rend(); ++entry) {
      out.push_back(entry->second);
    }
    return out;
  }

  /// The (n+1)-th entry from the left. The ordinal numbers the entries, not the positions:
  /// element(0) is the leftmost stored entry however far out its index lies.
  ///
  /// @throws std::out_of_range when fewer than n + 1 entries are stored.
  [[nodiscard]] element_type element(std::size_t n) const {
    const auto entry = _at_ordinal(n, false);
    return element_type{entry->first, entry->second};
  }

  /// @copydoc element(std::size_t) const
  /// @returns the index the entry sits at, not its ordinal.
  [[nodiscard]] index_type element_index(std::size_t n) const {
    return _at_ordinal(n, false)->first;
  }

  /// @copydoc element(std::size_t) const
  [[nodiscard]] T element_value(std::size_t n) const { return _at_ordinal(n, false)->second; }

  /// The (n+1)-th entry from the right, inverted_element(0) being the rightmost.
  ///
  /// @throws std::out_of_range when fewer than n + 1 entries are stored.
  [[nodiscard]] element_type inverted_element(std::size_t n) const {
    const auto entry = _at_ordinal(n, true);
    return element_type{entry->first, entry->second};
  }

  /// @copydoc inverted_element(std::size_t) const
  /// @returns the index the entry sits at, not its ordinal.
  [[nodiscard]] index_type inverted_element_index(std::size_t n) const {
    return _at_ordinal(n, true)->first;
  }

  /// @copydoc inverted_element(std::size_t) const
  [[nodiscard]] T inverted_element_value(std::size_t n) const {
    return _at_ordinal(n, true)->second;
  }

  /// The value `n` positions right of the leftmost entry, empty positions counting; a negative
  /// `n` walks left of it, into the default value.
  ///
  /// @throws std::out_of_range when nothing is stored, or the position leaves the index_type range.
  [[nodiscard]] T left_significant_value(index_type n) const {
    return get(_checked_subtract(_anchor(false), n));
  }

  /// The value `n` positions left of the rightmost entry.
  ///
  /// @copydoc left_significant_value(index_type) const
  [[nodiscard]] T right_significant_value(index_type n) const {
    return get(_checked_add(_anchor(true), n));
  }

  /// Iterates the entries descending by index, borrowing them, so
  /// `for (const auto& [index, value] : vector)` works without allocating.
  [[nodiscard]] typename container_type::const_iterator begin() const noexcept {
    return _entries.begin();
  }

  /// @copydoc begin() const
  [[nodiscard]] typename container_type::const_iterator end() const noexcept { return _entries.end(); }

  /// Builds from a sequence of elements, dropping those equal to the default and keeping the
  /// last of a repeated index.
  template <class InputIt>
  static SV_vector from_elements(InputIt first, InputIt last, const T& defaultValue) {
    SV_vector vector(defaultValue);
    for (; first != last; ++first) {
      vector.set(first->index, first->value);
    }
    return vector;
  }

  /// @copydoc from_elements(InputIt, InputIt, const T&)
  /// Uses `T{}` as the default value.
  template <class InputIt>
  static SV_vector from_elements(InputIt first, InputIt last) {
    return from_elements(first, last, T{});
  }

  /// @copydoc from_elements(InputIt, InputIt, const T&)
  /// The same, for a vector of elements.
  [[nodiscard]] static SV_vector from_elements(const std::vector<element_type>& elements,
                                               const T& defaultValue) {
    return from_elements(elements.begin(), elements.end(), defaultValue);
  }

  /// @copydoc from_elements(const std::vector<element_type>&, const T&)
  [[nodiscard]] static SV_vector from_elements(const std::vector<element_type>& elements) {
    return from_elements(elements.begin(), elements.end());
  }

 private:
  // Descending, so begin() is the leftmost entry and the inverted views walk it backwards.
  container_type _entries;
  T _default{};

  /// The outermost stored index: the leftmost, or the rightmost when `inverted`.
  ///
  /// @throws std::out_of_range when nothing is stored.
  index_type _anchor(bool inverted) const {
    if (_entries.empty()) {
      throw std::out_of_range("SV_vector holds no entries, so there is nothing to measure from");
    }
    return inverted ? std::prev(_entries.end())->first : _entries.begin()->first;
  }

  typename container_type::const_iterator _at_ordinal(std::size_t n, bool inverted) const {
    if (n >= _entries.size()) {
      throw std::out_of_range("SV_vector holds " + std::to_string(_entries.size()) +
                              " entries, so there is no entry " + std::to_string(n) +
                              (inverted ? " from the right" : " from the left"));
    }
    const auto distance = static_cast<typename container_type::difference_type>(n);
    return inverted ? std::prev(_entries.end(), distance + 1)
                    : std::next(_entries.begin(), distance);
  }

  /// `anchor + n`, refusing to wrap: signed overflow is undefined behaviour.
  static index_type _checked_add(index_type anchor, index_type n) {
    if (n > 0 && anchor > std::numeric_limits<index_type>::max() - n) {
      throw std::out_of_range("SV_vector position overflows index_type");
    }
    if (n < 0 && anchor < std::numeric_limits<index_type>::min() - n) {
      throw std::out_of_range("SV_vector position overflows index_type");
    }
    return anchor + n;
  }

  /// `anchor - n`, refusing to wrap.
  static index_type _checked_subtract(index_type anchor, index_type n) {
    if (n < 0 && anchor > std::numeric_limits<index_type>::max() + n) {
      throw std::out_of_range("SV_vector position overflows index_type");
    }
    if (n > 0 && anchor < std::numeric_limits<index_type>::min() + n) {
      throw std::out_of_range("SV_vector position overflows index_type");
    }
    return anchor - n;
  }

  void _prune() {
    for (auto entry = _entries.begin(); entry != _entries.end();) {
      if (entry->second == _default) {
        entry = _entries.erase(entry);  // erase returns the next iterator
      } else {
        ++entry;
      }
    }
  }
};

}  // namespace sv

#endif  // SV_VECTOR_HPP
