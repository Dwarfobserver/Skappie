
#include <catch.hpp>
#include "network/udp_socket.hpp"
#include "network/packet.hpp"
#include "messages/connect.hpp"
#include "messages/wrapper.hpp"


using namespace std::literals;

TEST_CASE("network asio synchronous UDP", "[network]") {
	using asio::ip::udp;
	try
	{
		asio::io_service ioService;

		const auto localhost = asio::ip::address::from_string("127.0.0.1");

		udp::socket sendingSocket(ioService);
		sendingSocket.open(udp::v4());
		sendingSocket.bind({ localhost, 54'321 });

		udp::socket receivingSocket(ioService);
		receivingSocket.open(udp::v4());
		receivingSocket.bind({ localhost, 54'322 });

		int buffer[4] {2, 5, 9, 12};
		sendingSocket.send_to(asio::buffer(buffer), { localhost, 54'322 });

		int bufferCopy[4];
		udp::endpoint remoteEndpoint;
		receivingSocket.receive_from(asio::buffer(bufferCopy), remoteEndpoint);

		REQUIRE(remoteEndpoint == udp::endpoint{ localhost, 54'321 });

		int diff = memcmp(buffer, bufferCopy, 4 * sizeof(int));
		REQUIRE(diff == 0);
	}
	catch (std::exception& e)
	{
		FAIL(e.what());
	}
}

TEST_CASE("network socket basics", "[network]") {

	std::byte buffer[100];
	const sk::bytes_span span{ buffer, sizeof(buffer) };

	const auto localhost = asio::ip::address::from_string("127.0.0.1");
	sk::net::address_type addr1{ localhost, 52'601 };
	sk::net::address_type addr2{ localhost, 52'602 };

	sk::net::udp_socket socket1;
	sk::net::udp_socket socket2;
	socket1.bind(addr1.port());
	socket2.bind(addr2.port());

	sk::msg::connect_request request;
	request.nickname = "xX_0bl1t3r4t0r_Xx";

	socket1.push({ request, span }, addr2);
	socket1.send_packets();

	sk::net::packet packet;
	sk::net::address_type addr;

	while (!socket2.try_get_packet(packet, addr)) {}

	REQUIRE(addr == addr1);
	auto wrappers = packet.extract_messages();

	REQUIRE(wrappers.size() == 1);

	sk::msg::connect_request requestCopy;
	wrappers[0].extract(requestCopy);

	REQUIRE(request.nickname == requestCopy.nickname);
}
