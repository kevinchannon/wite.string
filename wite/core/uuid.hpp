#pragma once

#include <wite/common/constructor_macros.hpp>
#include <wite/env/features.hpp>

#include <algorithm>
#include <array>
#include <compare>
#include <cstdint>
#include <random>
#include <string>
#include <string_view>
#include <tuple>
#include <regex>
#ifndef WITE_NO_EXCEPTIONS
#include <stdexcept>
#endif
#if _WITE_HAS_CONCEPTS
#include <concepts>
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
concept uuid_like = ((wite_uuid_like<T> or guid_like<T>) and sizeof(T) == 16);
#endif

struct uuid;

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD bool to_c_str(const Uuid_T& id, char* buffer, size_t max_buffer_length);
#else
_WITE_NODISCARD inline bool to_c_str(const uuid& id, char* buffer, size_t max_buffer_length);
#endif

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD std::string to_string(const Uuid_T& id);
#else
_WITE_NODISCARD inline std::string to_string(const uuid& id);
#endif

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD bool to_c_str(const Uuid_T& id, wchar_t* buffer, size_t max_buffer_length);
#else
_WITE_NODISCARD inline bool to_c_str(const uuid& id, wchar_t* buffer, size_t max_buffer_length);
#endif

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD std::wstring to_wstring(const Uuid_T& id);
#else
_WITE_NODISCARD inline std::wstring to_wstring(const uuid& id);
#endif

struct uuid {
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

#if _WITE_HAS_CONCEPTS
  template <std::invocable Engine_T>
  explicit uuid(Engine_T&& engine)
#else
  explicit uuid(std::default_random_engine& engine)
#endif
  {
    auto random_bits = std::uniform_int_distribution<uint64_t>{0x00, 0xFFFFFFFFFFFFFFFF};

    *reinterpret_cast<uint64_t*>(&data) = random_bits(engine);
    data[5]                             = static_cast<uint8_t>((data[5] & 0x0F) | 0x40);  // Version 4 UUID
    data[6]                             = static_cast<uint8_t>((data[6] & 0x3F) | 0x80);  // Variant 1 UUID

    *(reinterpret_cast<uint64_t*>(&data) + 1) = random_bits(engine);
  }

  explicit uuid(const std::string_view s) {
    if (s.length() < 37) {
#ifndef WITE_NO_EXCEPTIONS
      throw std::invalid_argument{"string to short to construct UUID"};
#else
      return;
#endif
    }
  }

  constexpr auto operator<=>(const uuid&) const noexcept = default;

  _WITE_NODISCARD bool into_c_str(char* out, size_t size) const noexcept { return to_c_str(*this, out, size); }
  _WITE_NODISCARD bool into_c_str(wchar_t* out, size_t size) const noexcept { return to_c_str(*this, out, size); }
  _WITE_NODISCARD std::string str() const { return to_string(*this); };
  _WITE_NODISCARD std::wstring wstr() const { return to_wstring(*this); };

  std::array<uint8_t, 16> data{};
};

constexpr static auto nulluuid = uuid{};

inline uuid make_uuid() {
  static thread_local auto random_engine = std::mt19937_64(std::random_device{}());
  return uuid{random_engine};
}

namespace detail {
  template <typename Char_T>
  _WITE_CONSTEVAL auto _uuid_format() {
    if constexpr (std::is_same_v<Char_T, char>) {
      return "%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
    } else {
      return L"%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X";
    }
  }

  template <typename Char_T>
  using _uuid_sprintf_t = int (*)(Char_T* const _Buffer, size_t const _BufferCount, Char_T const* const _Format, ...);

  template <typename Char_T>
  constexpr _uuid_sprintf_t<Char_T> _uuid_sprintf() {
    if constexpr (std::is_same_v<Char_T, char>) {
      return ::snprintf;
    } else {
      return ::swprintf;
    }
  }

  template <typename Char_T>
  struct _uuid_null_char {
    _WITE_CONSTEVAL static auto value() {
      if constexpr (std::is_same_v<Char_T, char>) {
        return '\0';
      } else {
        return L'\0';
      }
    }
  };

#if _WITE_HAS_CONCEPTS
  template <typename Char_T, wite::uuid_like Uuid_T>
  _WITE_NODISCARD bool _to_c_str(const Uuid_T& id, Char_T* buffer, size_t max_buffer_length)
#else
  template <typename Char_T>
  _WITE_NODISCARD bool _to_c_str(const uuid& id, Char_T* buffer, size_t max_buffer_length)
#endif
  {
    if (max_buffer_length < 37) {
      return false;
    }

    const uint32_t& data_1 = *reinterpret_cast<const uint32_t*>(&id);
    const uint16_t& data_2 = *reinterpret_cast<const uint16_t*>(reinterpret_cast<const uint8_t*>(&id) + 4);
    const uint16_t& data_3 = *reinterpret_cast<const uint16_t*>(reinterpret_cast<const uint8_t*>(&id) + 6);
    const uint8_t* data_4  = reinterpret_cast<const uint8_t*>(&id) + 8;

    std::ignore = detail::_uuid_sprintf<Char_T>()(buffer,
                                                max_buffer_length,
                                                detail::_uuid_format<Char_T>(),
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
    buffer[36]  = detail::_uuid_null_char<Char_T>::value();

    return true;
  }
}  // namespace detail

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD bool to_c_str(const Uuid_T& id, char* buffer, size_t max_buffer_length)
#else
_WITE_NODISCARD bool to_c_str(const uuid& id, char* buffer, size_t max_buffer_length)
#endif
{
  return detail::_to_c_str(id, buffer, max_buffer_length);
}

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD std::string to_string(const Uuid_T& id)
#else
_WITE_NODISCARD inline std::string to_string(const uuid& id)
#endif
{
  char buffer[39] = {};
  std::ignore     = to_c_str(id, buffer, 37);
  return {buffer};
}

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD bool to_c_str(const Uuid_T& id, wchar_t* buffer, size_t max_buffer_length)
#else
_WITE_NODISCARD inline bool to_c_str(const uuid& id, wchar_t* buffer, size_t max_buffer_length)
#endif
{
  return detail::_to_c_str(id, buffer, max_buffer_length);
}

#if _WITE_HAS_CONCEPTS
template <wite::uuid_like Uuid_T>
_WITE_NODISCARD std::wstring to_wstring(const Uuid_T& id)
#else
_WITE_NODISCARD inline std::wstring to_wstring(const uuid& id)
#endif
{
  wchar_t buffer[39] = {};
  std::ignore        = to_c_str(id, buffer, 37);
  return {buffer};
}

}  // namespace wite
