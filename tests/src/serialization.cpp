
#include <catch.hpp>
#include "bytes_span.hpp"
#include "messages/wrapper.hpp"
#include "messages/login.hpp"

using namespace std::literals;

TEST_CASE("serialization of primitive types", "[serialization]") {

	std::byte buffer[20];
	auto span = sk::bytes_span{ buffer };

	const float f = 1.2f;
	float fCopy;

	span << f;
	span.begin = buffer;
	span.peek(fCopy);

	REQUIRE(f == fCopy);
	REQUIRE(span.begin == buffer);

	const auto str = "Hello !"s;
	auto strCopy = "..."s;

	span << str;
	span.begin = buffer;
	span >> strCopy;

	REQUIRE(str == strCopy);
}

TEST_CASE("wrap a message", "[serialization]") {

	std::byte buffer[30];

	sk::msg::login_request msg;
	msg.stamp = 42;
	msg.nickname = "xX_0bl1t3r4t0r_Xx";

	sk::msg::wrapper wrapper;
	
	REQUIRE(wrapper.empty());

	wrapper.assign(msg, { buffer, buffer + sizeof(buffer) });

	REQUIRE(!wrapper.empty());
	REQUIRE(wrapper.tag() == sk::msg::tag_of<decltype(msg)>);

	sk::msg::login_request msgCopy;
	wrapper.extract(msgCopy);

	REQUIRE(msg.stamp    == msgCopy.stamp);
	REQUIRE(msg.nickname == msgCopy.nickname);
}
