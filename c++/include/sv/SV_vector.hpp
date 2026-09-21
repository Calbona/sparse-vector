#ifndef SV_VECTOR_HPP
#define SV_VECTOR_HPP

#include <cstddef>
#include <map>
#include <utility>
#include <vector>

#include "SV_element.hpp"

namespace sv {

/// A sparse vector: a mapping from integer index to arbitrary value, in which
/// only the positions that differ from a default value are stored.
///
/// Because only non-default positions are kept, a vector occupies O(k) memory for
/// k stored entries no matter how far apart the indices are, and whether they are
/// large or negative. Indices need not be contiguous, and an index holding no
/// explicit entry simply reports the default value.
template <class T = double>
class SV_vector {
 public:
  using index_type = sv::index_type;
  using value_type = T;
  using element_type = SV_element<T>;
  using container_type = std::map<index_type, T>;

  /// A vector whose empty positions read back as `T{}`; for `double` that is 0.0.
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

  /// The value reported for every position that has no explicit entry.
  ///
  /// There is deliberately no non-const overload, which would let a caller change
  /// the default without pruning; use set_default_value() for that.
  [[nodiscard]] const T& default_value() const noexcept { return _default; }

  /// Replaces the default value, immediately pruning every entry equal to it.
  ///
  /// Pruning is one-way: the previous default is not remembered.
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
  [[nodiscard]] std::size_t size() const noexcept { return _entries.size(); }

  /// Whether no entry is stored at all.
  [[nodiscard]] bool empty() const noexcept { return _entries.empty(); }

  /// Whether `index` has an explicitly stored entry.
  [[nodiscard]] bool has(index_type index) const { return _entries.find(index) != _entries.end(); }

  /// The value at `index`, or the default value when nothing is stored there.
  ///
  /// Reading is total: every integer, negative, huge or far outside anything ever
  /// written, returns a value rather than failing. Returned by value because the
  /// default branch has no entry to refer to.
  [[nodiscard]] T get(index_type index) const {
    const auto entry = _entries.find(index);
    if (entry == _entries.end()) {
      return _default;
    }
    return entry->second;
  }

  /// Inserts or updates `value` at `index`, returning `*this` so calls chain.
  ///
  /// Writing a value equal to the default removes whatever was there instead of
  /// storing it: that is what keeps the structure sparse.
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

  /// Removes the explicit entry at `index`, returning whether there was one.
  ///
  /// The position subsequently reads back as the default value.
  bool erase(index_type index) { return _entries.erase(index) != 0; }

  /// Removes every explicit entry, keeping the default value.
  void clear() noexcept { _entries.clear(); }

  /// The explicit entries, in ascending index order.
  [[nodiscard]] std::vector<element_type> elements() const {
    std::vector<element_type> out;
    out.reserve(_entries.size());
    for (const auto& entry : _entries) {
      out.push_back(element_type{entry.first, entry.second});
    }
    return out;
  }

  /// The indices holding an explicit entry, in ascending order.
  [[nodiscard]] std::vector<index_type> keys() const {
    std::vector<index_type> out;
    out.reserve(_entries.size());
    for (const auto& entry : _entries) {
      out.push_back(entry.first);
    }
    return out;
  }

  /// The stored values, in ascending index order.
  [[nodiscard]] std::vector<T> values() const {
    std::vector<T> out;
    out.reserve(_entries.size());
    for (const auto& entry : _entries) {
      out.push_back(entry.second);
    }
    return out;
  }

  /// Iterates the explicit entries in ascending index order, borrowing them.
  ///
  /// The pair is the map's own, so `for (const auto& [index, value] : vector)`
  /// works; reach for this instead of elements() when you do not want to allocate.
  [[nodiscard]] typename container_type::const_iterator begin() const noexcept {
    return _entries.begin();
  }

  /// @copydoc begin() const
  [[nodiscard]] typename container_type::const_iterator end() const noexcept { return _entries.end(); }

  /// Builds a vector from a sequence of elements.
  ///
  /// Entries equal to the default are dropped, and for a repeated index the last
  /// element wins, both because this goes through set().
  template <class InputIt>
  static SV_vector from_elements(InputIt first, InputIt last, const T& defaultValue) {
    SV_vector vector(defaultValue);
    for (; first != last; ++first) {
      vector.set(first->index, first->value);
    }
    return vector;
  }

  /// @copydoc from_elements(InputIt, InputIt, const T&)
  ///
  /// Uses `T{}` as the default value, matching the default constructor.
  template <class InputIt>
  static SV_vector from_elements(InputIt first, InputIt last) {
    return from_elements(first, last, T{});
  }

  /// @copydoc from_elements(InputIt, InputIt, const T&)
  [[nodiscard]] static SV_vector from(const std::vector<element_type>& elements,
                                      const T& defaultValue) {
    return from_elements(elements.begin(), elements.end(), defaultValue);
  }

  /// @copydoc from(const std::vector<element_type>&, const T&)
  [[nodiscard]] static SV_vector from(const std::vector<element_type>& elements) {
    return from_elements(elements.begin(), elements.end());
  }

 private:
  // Ordered on purpose: keys(), values(), elements() and begin()/end() all
  // guarantee ascending index order, which this container gives for free.
  container_type _entries;
  T _default{};

  /// Drops every entry equal to the current default.
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
