#pragma once

#include <wite/common/constructor_macros.hpp>
#include <wite/env/features.hpp>

#include <algorithm>
#include <array>
#include <compare>
#include <cstdint>
#include <random>
#include <string>
#include <tuple>
#if _WITE_FEATURE_USE_STD_FORMAT
#include <format>
#endif

namespace wite {

#ifdef _WITE_HAS_CONCEPTS
template <typename T>
concept wite_uuid_like = requires(T& t) { t.data; };

template <typename T>
concept guid_like = requires(T& t) {
                      t.Data1;
                      t.Data2;
                      t.Data3;
                      t.Data4[0];
                      t.Data4[1];
                      t.Data4[2];
                      t.Data4[3];
                      t.Data4[4];
                      t.Data4[5];
                      t.Data4[6];
                      t.Data4[7];
                    };
template <typename T>
concept uuid_like = (wite_uuid_like<T> || guid_like<T>);
#endif

struct uuid;

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD std::string to_string(const Uuid_T& id);
#else
_WITE_NODISCARD inline std::string to_string(const uuid& id);
#endif

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD bool to_c_str(const Uuid_T& id, char* buffer, size_t max_buffer_length);
#else
_WITE_NODISCARD inline bool to_c_str(const uuid& id, char* buffer, size_t max_buffer_length);
#endif

struct uuid {
  std::array<uint8_t, 16> data{};

  struct structured_data {
    unsigned long Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
  };

  WITE_DEFAULT_CONSTRUCTORS(uuid);

  uuid(unsigned long d1, unsigned short d2, unsigned short d3, std::array<unsigned char, 8> d4)
      : data{*((uint8_t*)(&d1)),
             *((uint8_t*)(&d1) + 1),
             *((uint8_t*)(&d1) + 2),
             *((uint8_t*)(&d1) + 3),
             *((uint8_t*)(&d2)),
             *((uint8_t*)(&d2) + 1),
             *((uint8_t*)(&d3)),
             *((uint8_t*)(&d3) + 1),
             d4[0],
             d4[1],
             d4[2],
             d4[3],
             d4[4],
             d4[5],
             d4[6],
             d4[7]} {}

  constexpr auto operator<=>(const uuid&) const noexcept = default;

  _WITE_NODISCARD bool into_c_str(char* out, size_t size) const noexcept { return to_c_str(*this, out, size); }
  _WITE_NODISCARD std::string str() const { return to_string(*this); };
};

inline uuid make_uuid() {
  static auto random_engine = std::mt19937_64(std::random_device{}());

  auto random_bits = std::uniform_int_distribution<uint64_t>{0x00, 0xFFFFFFFFFFFFFFFF};

  auto out                                = uuid{};
  *reinterpret_cast<uint64_t*>(&out.data) = random_bits(random_engine);
  out.data[5]                             = static_cast<uint8_t>((out.data[5] & 0x0F) | 0x40);  // Version 4 UUID
  out.data[6]                             = static_cast<uint8_t>((out.data[5] & 0x3F) | 0x80);  // Variant 1 UUID

  return out;
}

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD bool to_c_str(const Uuid_T& id, char* buffer, size_t max_buffer_length)
#else
_WITE_NODISCARD inline bool to_c_str(const uuid& id, char* buffer, size_t max_buffer_length)
#endif
{
  if (max_buffer_length < 39) {
    return false;
  }

  const unsigned long& data_1  = *reinterpret_cast<const unsigned long*>(&id);
  const unsigned short& data_2 = *reinterpret_cast<const unsigned short*>(reinterpret_cast<const unsigned char*>(&id) + 4);
  const unsigned short& data_3 = *reinterpret_cast<const unsigned short*>(reinterpret_cast<const unsigned char*>(&id) + 6);
  const unsigned char* data_4  = reinterpret_cast<const unsigned char*>(&id) + 8;

  std::ignore = ::snprintf(buffer,
                           max_buffer_length,
                           "{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
                           data_1,
                           data_2,
                           data_3,
                           data_4[0],
                           data_4[1],
                           data_4[2],
                           data_4[3],
                           data_4[4],
                           data_4[5],
                           data_4[6],
                           data_4[7]);
  buffer[38]  = '\0';

  return true;
}

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD std::string to_string(const Uuid_T& id)
#else
_WITE_NODISCARD inline std::string to_string(const uuid& id)
#endif
{
  char buffer[39] = {};
  std::ignore     = to_c_str(id, buffer, 39);
  return {buffer};
}

}  // namespace wite
