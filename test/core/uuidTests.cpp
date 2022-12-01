#include <wite/core/io.hpp>
#include <wite/core/uuid.hpp>

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <sstream>
#include <iostream>

using namespace wite;
using namespace std::chrono_literals;

TEST_CASE("Uuid tests", "[core]") {

  SECTION("create random UUID") {
    const auto id_1 = make_uuid();
    const auto id_2 = make_uuid();
    REQUIRE(id_1 != id_2);
  }

  SECTION("Making a UUID doesn't take too long") {
    const auto start = std::chrono::high_resolution_clock::now();
  
    for (auto i = 0u; i < 100'000; ++i) {
      const auto id = make_uuid();
      (void)id;
    }
  
    const auto duration = std::chrono::high_resolution_clock::now() - start;
  
    REQUIRE(100ms > std::chrono::duration_cast<std::chrono::milliseconds>(duration));
  }
}

TEST_CASE("Uuid IO tests", "[core]") {
  SECTION("Uuid can be inserted into a stream") {
    const auto g = uuid{0x01234567, 0x89AB, 0xCDEF, {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF}};
    std::stringstream ss;
  
    ss << g;
  
    // REQUIRE("{01234567-89AB-CDEF-0123-456789ABCDEF}" == ss.str());
  }
}