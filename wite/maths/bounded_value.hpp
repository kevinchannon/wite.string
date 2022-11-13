#pragma once

#include <wite/env/environment.hpp>

///////////////////////////////////////////////////////////////////////////////

namespace wite::maths {

///////////////////////////////////////////////////////////////////////////////

template<typename Value_T>
class bounded_value{
 public:
  explicit bounded_value(Value_T value) : _value{std::move(value)} {}

  _WITE_NODISCARD constexpr const Value_T& value() const noexcept { return _value; }

  void value(const Value_T& new_value) noexcept { _value = new_value; }

 private:
  Value_T _value;
};

///////////////////////////////////////////////////////////////////////////////

}

///////////////////////////////////////////////////////////////////////////////
