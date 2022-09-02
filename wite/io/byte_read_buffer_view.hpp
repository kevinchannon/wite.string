#pragma once

#include <wite/io/byte_buffer_read.hpp>
#include <wite/io/encoding.hpp>
#include <wite/io/types.hpp>
#include <wite/io/concepts.hpp>

#include <bit>
#include <cstddef>
#include <span>
#include <type_traits>
#include <iterator>

///////////////////////////////////////////////////////////////////////////////

namespace wite::io {

///////////////////////////////////////////////////////////////////////////////

struct byte_read_buffer_view {
  explicit byte_read_buffer_view(std::span<const std::byte> buf) : data{std::move(buf)}, read_position{data.begin()} {}

  byte_read_buffer_view(std::span<const std::byte> buf, typename std::span<const std::byte>::size_type offset)
      : data{std::move(buf)}, read_position{std::next(data.begin(), offset)} {}

  std::span<const std::byte> data;
  std::span<const std::byte>::iterator read_position;
};

///////////////////////////////////////////////////////////////////////////////

template <typename Value_T>
requires is_buffer_readable<Value_T> and (not std::is_base_of_v<io::encoding, Value_T>)
Value_T read(byte_read_buffer_view& buffer) {
  const auto out = read<Value_T>({buffer.read_position, buffer.data.end()});
  std::advance(buffer.read_position, sizeof(Value_T));

  return out;
}

///////////////////////////////////////////////////////////////////////////////

template <typename Value_T>
requires is_buffer_writeable<Value_T> and (not std::is_base_of_v<io::encoding, Value_T>)
read_result_t<Value_T> try_read(byte_read_buffer_view& buffer) {
  const auto out = try_read<Value_T>({buffer.read_position, buffer.data.end()});
  std::advance(buffer.read_position, sizeof(Value_T));

  return out;
}

///////////////////////////////////////////////////////////////////////////////

template <typename Value_T>
requires is_buffer_writeable<Value_T> and std::is_base_of_v<io::encoding, Value_T>
read_result_t<typename Value_T::value_type>
try_read(
    byte_read_buffer_view& buffer) {
  const auto out = try_read<Value_T>({buffer.read_position, buffer.data.end()});
  std::advance(buffer.read_position, sizeof(Value_T));

  return out;
}

///////////////////////////////////////////////////////////////////////////////

template <typename Value_T>
requires is_buffer_writeable<Value_T> and (not std::is_base_of_v<io::encoding, Value_T>)
Value_T read(byte_read_buffer_view& buffer, std::endian endienness) {
  const auto out = read<Value_T>({buffer.read_position, buffer.data.end()}, endienness);
  std::advance(buffer.read_position, sizeof(out));

  return out;
}

///////////////////////////////////////////////////////////////////////////////

template <typename Value_T>
requires is_buffer_writeable<Value_T> and std::is_base_of_v<io::encoding, Value_T>
typename Value_T::value_type read(byte_read_buffer_view& buffer) {
  const auto out = read<Value_T>({buffer.read_position, buffer.data.end()});
  std::advance(buffer.read_position, sizeof(out));

  return out;
}

///////////////////////////////////////////////////////////////////////////////

}  // namespace wite::io

///////////////////////////////////////////////////////////////////////////////