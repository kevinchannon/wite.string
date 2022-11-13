#pragma once

#include <wite/env/environment.hpp>

#include <wite/maths/value_range.hpp>

#include <limits>

///////////////////////////////////////////////////////////////////////////////

namespace wite::maths {

///////////////////////////////////////////////////////////////////////////////

template <typename Value_T>
class bounded_value {
 public:
  using value_type = Value_T;
  using bound_type = value_range<value_type>;

  constexpr bounded_value(Value_T value,
                          bound_type bounds = bound_type{std::numeric_limits<value_type>::min(),
                                                         std::numeric_limits<value_type>::max()})
      : _value{std::move(value)}, _bounds{std::move(bounds)} {}

  _WITE_NODISCARD constexpr const Value_T& value() const noexcept { return _value; }

  bounded_value& value(const Value_T& new_value) noexcept { 
    _value = new_value;
    return *this;
  }

  _WITE_NODISCARD constexpr const bound_type& bounds() const noexcept { return _bounds; }
  bounded_value& bounds(const bound_type& new_bounds) {
    _bounds = new_bounds;
    return *this;
  }

  _WITE_NODISCARD constexpr bool is_in_bounds() const noexcept { return _bounds.contains(_value); }

  bounded_value& clamp() noexcept {
    _value = _bounds.clamp(_value);
    return *this;
  }

 private:
  Value_T _value{};
  bound_type _bounds{};
};

///////////////////////////////////////////////////////////////////////////////

}  // namespace wite::maths

///////////////////////////////////////////////////////////////////////////////