
#include <catch.hpp>
#include "bytes_span.hpp"
#include "messages/wrapper.hpp"
#include "messages/connect.hpp"
#include "network/packet.hpp"

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

	sk::msg::connect_request msg;
	msg.type = sk::msg::connect_request::new_account;
	msg.nickname = "xX_0bl1t3r4t0r_Xx";

	sk::msg::wrapper wrapper;
	
	REQUIRE(wrapper.empty());

	wrapper.assign(msg, { buffer, buffer + sizeof(buffer) });

	REQUIRE(!wrapper.empty());
	REQUIRE(wrapper.tag() == sk::msg::tag_of<decltype(msg)>);

	sk::msg::connect_request msgCopy;
	wrapper.extract(msgCopy);

	REQUIRE(msg.type     == msgCopy.type);
	REQUIRE(msg.nickname == msgCopy.nickname);
}

TEST_CASE("packetage messages", "[serialization]") {

	std::byte buffer[70];
	sk::bytes_span span{ buffer, sizeof(buffer) };

	// Messages (2 must fit in the buffer, but not 3 - including the packet header)

	sk::msg::connect_request msg1;
	msg1.stamp = 42;
	msg1.nickname = "xX_0bl1t3r4t0r_Xx";

	sk::msg::connect_response msg2;
	msg2.stamp = 42;
	msg2.accepted = false;
	msg2.reason = "Ugly pseudo";

	// Wrappers

	sk::msg::wrapper wrapper1;
	wrapper1.assign(msg1, span);

	sk::msg::wrapper wrapper2;
	wrapper2.assign(msg2, span);

	// Packet

	sk::net::packet packet;
	packet.set_max_size(sizeof(buffer));

	REQUIRE(packet.try_push(wrapper1));
	REQUIRE(packet.try_push(wrapper2));
	REQUIRE(!packet.try_push(wrapper1));

	// Serialization

	span << packet;
	REQUIRE(span.begin - buffer == sk::serialized_size(packet));

	span.begin = buffer;

	sk::net::packet packetCopy;
	span >> packetCopy;

	REQUIRE(packetCopy.messages_count() == 2);

	auto wrappers = packetCopy.extract_messages();
	REQUIRE(wrappers.size() == 2);

	// Messages extraction

	sk::msg::connect_request msg1Copy;
	wrappers[0].extract(msg1Copy);

	sk::msg::connect_response msg2Copy;
	wrappers[1].extract(msg2Copy);

	REQUIRE(msg1Copy.stamp == msg1.stamp);
	REQUIRE(msg1Copy.nickname == msg1.nickname);

	REQUIRE(msg2Copy.stamp == msg2.stamp);
	REQUIRE(msg2Copy.accepted == msg2.accepted);
	REQUIRE(msg2Copy.reason == msg2.reason);
}
