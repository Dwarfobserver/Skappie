
#include <catch.hpp>
#include <bytes_span.hpp>

using namespace std::literals;

TEST_CASE("serialisation std::string", "[serialisation]") {

	std::byte buffer[20];
	auto span = sk::bytes_span{ buffer };

	const auto str = "Hello !"s;
	auto strCopy = "..."s;

	span << str;
	span.begin = buffer;
	span >> strCopy;

	REQUIRE(str == strCopy);
}
