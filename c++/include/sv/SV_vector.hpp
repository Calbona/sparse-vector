#ifndef SV_VECTOR_HPP
#define SV_VECTOR_HPP

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <string>
#include <unordered_map>
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
  /// The custom equality predicate, called with the value under test first and the current
  /// default second. An empty function means `operator==`.
  using equality_type = std::function<bool(const T&, const T&)>;
  /// The element container: a hash map, so a position is reached in one probe rather than a walk.
  /// It holds no order, so the descending order the API promises comes from `_ordered`.
  using container_type = std::unordered_map<index_type, T>;

  /// Borrows the stored elements descending by index, largest leftmost. Dereferences to the
  /// container's own `std::pair<const index_type, T>`, so iterating copies nothing.
  class const_iterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = typename container_type::value_type;
    using difference_type = std::ptrdiff_t;
    using pointer = const value_type*;
    using reference = const value_type&;

    const_iterator() = default;

    [[nodiscard]] reference operator*() const noexcept { return **_slot; }
    [[nodiscard]] pointer operator->() const noexcept { return *_slot; }

    const_iterator& operator++() noexcept {
      ++_slot;
      return *this;
    }

    const_iterator operator++(int) noexcept {
      const_iterator copy = *this;
      ++_slot;
      return copy;
    }

    [[nodiscard]] friend bool operator==(const const_iterator& left,
                                         const const_iterator& right) noexcept {
      return left._slot == right._slot;
    }

    [[nodiscard]] friend bool operator!=(const const_iterator& left,
                                         const const_iterator& right) noexcept {
      return left._slot != right._slot;
    }

   private:
    friend class SV_vector;

    explicit const_iterator(const value_type* const* slot) noexcept : _slot(slot) {}

    /// Where the element sits in the vector `_ordered`, not in the hash map.
    const value_type* const* _slot = nullptr;
  };

  /// A vector whose empty positions read back as `T{}`.
  SV_vector() = default;

  /// A vector whose empty positions read back as `defaultValue`.
  explicit SV_vector(const T& defaultValue) : _default(defaultValue) {}

  /// @copydoc SV_vector(const T&)
  explicit SV_vector(T&& defaultValue) : _default(std::move(defaultValue)) {}

  /// @copydoc SV_vector(const T&)
  /// The copy carries no ordered index of its own: `_ordered` points into whichever vector built
  /// it, so the copy starts out stale and rebuilds on its first ordered read.
  SV_vector(const SV_vector& other)
      : _elements(other._elements),
        _default(other._default),
        _equality(other._equality),
        _ordered_valid(_elements.empty()) {}

  /// @copydoc SV_vector(const SV_vector&)
  SV_vector(SV_vector&& other) noexcept
      : _elements(std::move(other._elements)),
        _default(std::move(other._default)),
        _equality(std::move(other._equality)),
        _ordered_valid(_elements.empty()) {}

  /// @copydoc SV_vector(const SV_vector&)
  SV_vector& operator=(const SV_vector& other) {
    if (this != &other) {
      _elements = other._elements;
      _default = other._default;
      _equality = other._equality;
      _invalidate_ordered();
    }
    return *this;
  }

  /// @copydoc SV_vector(const SV_vector&)
  SV_vector& operator=(SV_vector&& other) noexcept {
    if (this != &other) {
      _elements = std::move(other._elements);
      _default = std::move(other._default);
      _equality = std::move(other._equality);
      _invalidate_ordered();
    }
    return *this;
  }

  ~SV_vector() = default;

  /// The value reported for every position with no explicit element.
  ///
  /// There is deliberately no non-const overload; use set_default_value(), which prunes.
  [[nodiscard]] const T& get_default_value() const noexcept { return _default; }

  /// Replaces the default value, immediately pruning every element equal to it and returning
  /// whether anything was pruned. Pruning is one-way: the previous default is not remembered.
  bool set_default_value(const T& next) {
    _default = next;
    return _prune();
  }

  /// @copydoc set_default_value(const T&)
  bool set_default_value(T&& next) {
    _default = std::move(next);
    return _prune();
  }

  /// The predicate replacing `operator==`, empty when there is none.
  [[nodiscard]] const equality_type& get_equality() const noexcept { return _equality; }

  /// Replaces the predicate, immediately pruning every element it calls equal to the default and
  /// returning whether anything was pruned. An empty function restores `operator==`.
  bool set_equality(equality_type next) {
    _equality = std::move(next);
    return _prune();
  }

  /// The number of explicitly stored elements.
  [[nodiscard]] std::size_t get_element_amount() const noexcept { return _elements.size(); }

  /// The distance between the leftmost and rightmost elements, both included; zero when empty.
  ///
  /// @throws std::out_of_range when the span leaves the index_type range.
  [[nodiscard]] index_type get_significant_dimension() const {
    _rebuild_ordered();
    if (_ordered.empty()) {
      return 0;
    }
    return _checked_add(_checked_subtract(_ordered.front()->first, _ordered.back()->first), 1);
  }

  /// How far the vector reaches above zero: the leftmost index itself, or zero.
  [[nodiscard]] index_type get_plus_dimension() const {
    _rebuild_ordered();
    if (_ordered.empty()) {
      return 0;
    }
    const index_type leftmost = _ordered.front()->first;
    return leftmost > 0 ? leftmost : 0;
  }

  /// How far the vector reaches below zero: the rightmost index negated, or zero.
  ///
  /// @throws std::out_of_range when the negation leaves the index_type range.
  [[nodiscard]] index_type get_minus_dimension() const {
    _rebuild_ordered();
    if (_ordered.empty()) {
      return 0;
    }
    const index_type rightmost = _ordered.back()->first;
    return rightmost < 0 ? _checked_subtract(0, rightmost) : 0;
  }

  /// The value at `index`, or the default value when nothing is stored there. Reading is
  /// total, so it returns by value: the default branch has no element to refer to.
  [[nodiscard]] T get(index_type index) const {
    const auto element = _elements.find(index);
    if (element == _elements.end()) {
      return _default;
    }
    return element->second;
  }

  /// Inserts or updates `value` at `index`, returning `*this` so calls chain. Writing the
  /// default removes whatever was there instead of storing it.
  SV_vector& set(index_type index, const T& value) {
    if (_equals(value)) {
      if (_elements.erase(index) != 0) {
        _invalidate_ordered();
      }
    } else if (_elements.insert_or_assign(index, value).second) {
      // Only a new index changes the order; overwriting one leaves the ordered index alone.
      _invalidate_ordered();
    }
    return *this;
  }

  /// @copydoc set(index_type, const T&)
  SV_vector& set(index_type index, T&& value) {
    if (_equals(value)) {
      if (_elements.erase(index) != 0) {
        _invalidate_ordered();
      }
    } else if (_elements.insert_or_assign(index, std::move(value)).second) {
      _invalidate_ordered();
    }
    return *this;
  }

  /// Resets the element at `index`, returning whether there was one; it then reads as the default.
  bool reset_value(index_type index) {
    if (_elements.erase(index) == 0) {
      return false;
    }
    _invalidate_ordered();
    return true;
  }

  /// Resets every explicit element, keeping the default value, and returns whether there was any.
  bool reset_vector() noexcept {
    const bool cleared = !_elements.empty();
    _elements.clear();
    _invalidate_ordered();
    return cleared;
  }

  /// The explicit elements, descending by index.
  [[nodiscard]] std::vector<element_type> elements() const {
    _rebuild_ordered();
    std::vector<element_type> out;
    out.reserve(_ordered.size());
    for (const auto* element : _ordered) {
      out.push_back(element_type{element->first, element->second});
    }
    return out;
  }

  /// The same elements, ascending by index.
  [[nodiscard]] std::vector<element_type> inverted_elements() const {
    _rebuild_ordered();
    std::vector<element_type> out;
    out.reserve(_ordered.size());
    for (auto element = _ordered.rbegin(); element != _ordered.rend(); ++element) {
      out.push_back(element_type{(*element)->first, (*element)->second});
    }
    return out;
  }

  /// The stored indices, descending.
  [[nodiscard]] std::vector<index_type> indexes() const {
    _rebuild_ordered();
    std::vector<index_type> out;
    out.reserve(_ordered.size());
    for (const auto* element : _ordered) {
      out.push_back(element->first);
    }
    return out;
  }

  /// The same indices, ascending.
  [[nodiscard]] std::vector<index_type> inverted_indexes() const {
    _rebuild_ordered();
    std::vector<index_type> out;
    out.reserve(_ordered.size());
    for (auto element = _ordered.rbegin(); element != _ordered.rend(); ++element) {
      out.push_back((*element)->first);
    }
    return out;
  }

  /// The stored values, descending by index.
  [[nodiscard]] std::vector<T> values() const {
    _rebuild_ordered();
    std::vector<T> out;
    out.reserve(_ordered.size());
    for (const auto* element : _ordered) {
      out.push_back(element->second);
    }
    return out;
  }

  /// The same values, ascending by index.
  [[nodiscard]] std::vector<T> inverted_values() const {
    _rebuild_ordered();
    std::vector<T> out;
    out.reserve(_ordered.size());
    for (auto element = _ordered.rbegin(); element != _ordered.rend(); ++element) {
      out.push_back((*element)->second);
    }
    return out;
  }

  /// The (n+1)-th element from the left. The ordinal numbers the elements, not the positions:
  /// element(0) is the leftmost stored element however far out its index lies.
  ///
  /// @throws std::out_of_range when fewer than n + 1 elements are stored.
  [[nodiscard]] element_type element(std::size_t n) const {
    const auto* found = _at_ordinal(n, false);
    return element_type{found->first, found->second};
  }

  /// @copydoc element(std::size_t) const
  /// @returns the index the element sits at, not its ordinal.
  [[nodiscard]] index_type element_index(std::size_t n) const { return _at_ordinal(n, false)->first; }

  /// @copydoc element(std::size_t) const
  [[nodiscard]] T element_value(std::size_t n) const { return _at_ordinal(n, false)->second; }

  /// The (n+1)-th element from the right, inverted_element(0) being the rightmost.
  ///
  /// @throws std::out_of_range when fewer than n + 1 elements are stored.
  [[nodiscard]] element_type inverted_element(std::size_t n) const {
    const auto* found = _at_ordinal(n, true);
    return element_type{found->first, found->second};
  }

  /// @copydoc inverted_element(std::size_t) const
  /// @returns the index the element sits at, not its ordinal.
  [[nodiscard]] index_type inverted_element_index(std::size_t n) const {
    return _at_ordinal(n, true)->first;
  }

  /// @copydoc inverted_element(std::size_t) const
  [[nodiscard]] T inverted_element_value(std::size_t n) const {
    return _at_ordinal(n, true)->second;
  }

  /// The value `n` positions right of the leftmost element, empty positions counting; a negative
  /// `n` walks left of it, into the default value.
  ///
  /// @throws std::out_of_range when nothing is stored, or the position leaves the index_type range.
  [[nodiscard]] T left_significant_value(index_type n) const {
    return get(_checked_subtract(_anchor(false), n));
  }

  /// The value `n` positions left of the rightmost element.
  ///
  /// @copydoc left_significant_value(index_type) const
  [[nodiscard]] T right_significant_value(index_type n) const {
    return get(_checked_add(_anchor(true), n));
  }

  /// Whether `other` holds the same values as *this, every comparison made by `_equality`: the two
  /// default values first, then each index either vector stores an element at, a position holding
  /// nothing contributing its own vector's default.
  ///
  /// Never throws. Two `NaN` defaults compare unequal, so a vector holding one is not equal to
  /// itself. The three properties of equality hold only as far as `_equality` does — an asymmetric
  /// or non-reflexive predicate gives an asymmetric or non-reflexive answer, the library never
  /// correcting it.
  [[nodiscard]] bool is_equal_to(const SV_vector& other) const {
    if (!_equals_values(other._default, _default)) {
      return false;
    }
    _rebuild_ordered();
    other._rebuild_ordered();
    auto mine = _ordered.begin();
    auto theirs = other._ordered.begin();
    while (mine != _ordered.end() || theirs != other._ordered.end()) {
      if (theirs == other._ordered.end() ||
          (mine != _ordered.end() && (*mine)->first > (*theirs)->first)) {
        if (!_equals_values(other._default, (*mine)->second)) {
          return false;
        }
        ++mine;
      } else if (mine == _ordered.end() || (*theirs)->first > (*mine)->first) {
        if (!_equals_values((*theirs)->second, _default)) {
          return false;
        }
        ++theirs;
      } else {
        if (!_equals_values((*theirs)->second, (*mine)->second)) {
          return false;
        }
        ++mine;
        ++theirs;
      }
    }
    return true;
  }

  /// The elements of *this that differ from `other`, descending by index, each carrying this
  /// vector's value at that index — the default value itself wherever nothing is stored there.
  ///
  /// Not a symmetric difference, and not guaranteed to be empty for `a.differences(a)`: a stored
  /// `NaN` differs from itself, since `NaN != NaN`.
  ///
  /// @throws std::invalid_argument when the two default values do not compare equal under
  /// `_equality`, so there is no sense in which the vectors can be compared.
  [[nodiscard]] std::vector<element_type> differences(const SV_vector& other) const {
    if (!_equals_values(other._default, _default)) {
      throw std::invalid_argument(
          "SV_vector default values are not equal, so the two vectors cannot be compared");
    }
    _rebuild_ordered();
    other._rebuild_ordered();
    std::vector<element_type> differing;
    auto mine = _ordered.begin();
    auto theirs = other._ordered.begin();
    while (mine != _ordered.end() || theirs != other._ordered.end()) {
      if (theirs == other._ordered.end() ||
          (mine != _ordered.end() && (*mine)->first > (*theirs)->first)) {
        if (!_equals_values(other._default, (*mine)->second)) {
          differing.push_back(element_type{(*mine)->first, (*mine)->second});
        }
        ++mine;
      } else if (mine == _ordered.end() || (*theirs)->first > (*mine)->first) {
        if (!_equals_values((*theirs)->second, _default)) {
          differing.push_back(element_type{(*theirs)->first, _default});
        }
        ++theirs;
      } else {
        if (!_equals_values((*theirs)->second, (*mine)->second)) {
          differing.push_back(element_type{(*mine)->first, (*mine)->second});
        }
        ++mine;
        ++theirs;
      }
    }
    return differing;
  }

  /// Iterates the elements descending by index, borrowing them, so
  /// `for (const auto& [index, value] : vector)` works without copying.
  [[nodiscard]] const_iterator begin() const {
    _rebuild_ordered();
    return const_iterator{_ordered.data()};
  }

  /// @copydoc begin() const
  [[nodiscard]] const_iterator end() const {
    _rebuild_ordered();
    return const_iterator{_ordered.data() + _ordered.size()};
  }

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
  container_type _elements;
  T _default{};
  equality_type _equality;

  /// The stored elements, descending by index, holding pointers into `_elements` rather than
  /// copies. Rebuilt by the first ordered read after the set of stored indices changes.
  ///
  /// This cache is why two threads must not read one `const SV_vector` at the same time: an
  /// ordered read may rebuild it. Rebuilding never happens during a plain `get()`.
  mutable std::vector<const typename container_type::value_type*> _ordered;
  mutable bool _ordered_valid = true;

  /// Whether `left` and `right` are equal, by `_equality` when set and `operator==` otherwise.
  ///
  /// The predicate decides every comparison in this class. Which value goes in the first slot is
  /// the caller's choice, and a comparison against another vector always puts its value first.
  [[nodiscard]] bool _equals_values(const T& left, const T& right) const {
    return _equality ? _equality(left, right) : left == right;
  }

  /// Whether `value` equals the default, by `_equality` when set and `operator==` otherwise.
  [[nodiscard]] bool _equals(const T& value) const { return _equals_values(value, _default); }

  /// Declares the ordered index stale, dropping the pointers it holds. The next ordered read
  /// rebuilds it before touching any of them.
  void _invalidate_ordered() noexcept {
    _ordered.clear();
    _ordered_valid = false;
  }

  /// Fills the ordered index when it has gone stale. Every ordered read calls this first.
  void _rebuild_ordered() const {
    if (_ordered_valid) {
      return;
    }
    _ordered.reserve(_elements.size());
    for (const auto& element : _elements) {
      _ordered.push_back(&element);
    }
    std::sort(_ordered.begin(), _ordered.end(), _leftmost_first);
    _ordered_valid = true;
  }

  /// Orders two stored elements largest index first.
  [[nodiscard]] static bool _leftmost_first(const typename container_type::value_type* left,
                                            const typename container_type::value_type* right) {
    return left->first > right->first;
  }

  /// The outermost stored index: the leftmost, or the rightmost when `inverted`.
  ///
  /// @throws std::out_of_range when nothing is stored.
  index_type _anchor(bool inverted) const {
    _rebuild_ordered();
    if (_ordered.empty()) {
      throw std::out_of_range("SV_vector holds no elements, so there is nothing to measure from");
    }
    return inverted ? _ordered.back()->first : _ordered.front()->first;
  }

  /// The element at ordinal `n`, counted from the left, or from the right when `inverted`.
  ///
  /// @throws std::out_of_range when fewer than n + 1 elements are stored.
  [[nodiscard]] const typename container_type::value_type* _at_ordinal(std::size_t n,
                                                                      bool inverted) const {
    _rebuild_ordered();
    if (n >= _ordered.size()) {
      throw std::out_of_range("SV_vector holds " + std::to_string(_ordered.size()) +
                              " elements, so there is no element " + std::to_string(n) +
                              (inverted ? " from the right" : " from the left"));
    }
    return inverted ? _ordered[_ordered.size() - 1 - n] : _ordered[n];
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

  /// Drops every element equal to the default, returning whether anything was dropped.
  bool _prune() {
    bool pruned = false;
    for (auto element = _elements.begin(); element != _elements.end();) {
      if (_equals(element->second)) {
        element = _elements.erase(element);  // erase returns the next iterator
        pruned = true;
      } else {
        ++element;
      }
    }
    if (pruned) {
      _invalidate_ordered();
    }
    return pruned;
  }
};

}  // namespace sv

#endif  // SV_VECTOR_HPP
