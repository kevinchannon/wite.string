#pragma once

#include <wite/env/environment.hpp>
#include <wite/core/result.hpp>

#include <array>
#include <cstddef>
#include <vector>

///////////////////////////////////////////////////////////////////////////////

namespace wite::io {

using byte = WITE_BYTE;

///////////////////////////////////////////////////////////////////////////////

template <size_t N>
using static_byte_buffer = std::array<byte, N>;

using dynamic_byte_buffer = std::vector<byte>;

///////////////////////////////////////////////////////////////////////////////

enum class read_error {
  insufficient_buffer
};

template<typename Value_T>
using read_result_t = wite::result<Value_T, read_error>;

///////////////////////////////////////////////////////////////////////////////

enum class write_error {
  insufficient_buffer
};

using write_result_t = wite::result<size_t, write_error>;

///////////////////////////////////////////////////////////////////////////////

}  // namespace wite::io

///////////////////////////////////////////////////////////////////////////////

