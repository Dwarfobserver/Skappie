
#include <catch.hpp>
#include "core.hpp"

using namespace std::literals;

namespace {
	char const* streamableChar = "<Streamable>";
	struct _streamable_t {};
	std::ostringstream& operator<<(std::ostringstream& oss, _streamable_t) {
		oss << streamableChar;
		return oss;
	}

	char const* stringableChar = "<Streamable>";
	struct _stringable_t {};
	std::string to_string(_stringable_t) {
		return stringableChar;
	}
}

TEST_CASE("parsing with make_string", "[parsing]") {
	static_assert(std::is_constructible_v<std::string, char const*>);
	static_assert(sk::traits::has_std_to_string<int>);
	static_assert(sk::traits::is_string_streamable<_streamable_t>);
	static_assert(sk::traits::has_to_string<_stringable_t>);

	REQUIRE(sk::make_string("derp") == "derp"s);
	REQUIRE(sk::make_string(2) == "2"s);
	REQUIRE(sk::make_string(_streamable_t{}) == streamableChar);
	REQUIRE(sk::make_string(_stringable_t{}) == stringableChar);
}

