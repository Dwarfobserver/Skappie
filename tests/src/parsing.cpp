
#include <catch.hpp>
#include "utils.hpp"
#include "file_parser.hpp"

using namespace std::literals;

namespace {
	char const* stringableChar = "<Streamable>";
	struct stringable_t {};
	std::string to_string(stringable_t) {
		return stringableChar;
	}

	char const* streamableChar = "<Streamable>";
	struct streamable_t {};
	std::ostringstream& operator<<(std::ostringstream& oss, streamable_t) {
		oss << streamableChar;
		return oss;
	}
}

TEST_CASE("parsing with make_string", "[parsing]") {
	static_assert(std::is_constructible_v<std::string, char const*>);
	REQUIRE(sk::make_string("derp") == "derp"s);

	static_assert(sk::traits::has_std_to_string<int>);
	REQUIRE(sk::make_string(2) == "2"s);

	static_assert(sk::traits::has_to_string<stringable_t>);
	REQUIRE(sk::make_string(stringable_t{}) == stringableChar);

	static_assert(sk::traits::is_string_streamable<streamable_t>);
	REQUIRE(sk::make_string(streamable_t{}) == streamableChar);
}

TEST_CASE("parsing file", "[parsing]") {
	char const* path = "__parsing_test__";
	
	auto parser = sk::file_parser::try_open(path);
	REQUIRE(!parser.is_open());

	parser = sk::file_parser::try_create(path);
	REQUIRE(parser.is_open());

	sk::file_line line;
	std::string str;
	int val;
	std::vector<std::string> strings;

	line.write_comment("Hello world");
	line.read_comment(str);
	REQUIRE(str == "Hello world");
	parser.content().push_back(line);

	line.write_section(2, "Section");
	line.read_section(val, str);
	REQUIRE(val == 2);
	REQUIRE(str == "Section");
	parser.content().push_back(line);

	line.write_values({"1", "2", "3"});
	line.read_values(strings);
	REQUIRE(strings == std::vector<std::string>{"1", "2", "3"});
	parser.content().push_back(line);
	
	parser.save();

	auto parserCopy = sk::file_parser::try_open(path);
	REQUIRE(parserCopy.is_open());

	REQUIRE(parser.content() == parserCopy.content());

	REQUIRE(sk::file_parser::try_remove(path));
}

