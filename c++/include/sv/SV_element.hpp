#ifndef SV_ELEMENT_HPP
#define SV_ELEMENT_HPP

#include <cstdint>

namespace sv {

/// The type of every index, negative ones included.
using index_type = std::int64_t;

/// A single stored position: the serialization shape, so a round trip has to carry the
/// default value alongside.
template <class T = double>
struct SV_element {
  index_type index;
  T value;

  friend bool operator==(const SV_element& a, const SV_element& b) {
    return a.index == b.index && a.value == b.value;
  }

  friend bool operator!=(const SV_element& a, const SV_element& b) { return !(a == b); }
};

}  // namespace sv

#endif  // SV_ELEMENT_HPP
