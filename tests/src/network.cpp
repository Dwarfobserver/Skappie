
#include <catch.hpp>
#include <asio/io_service.hpp>
#include <asio/ip/udp.hpp>
#include <asio/buffer.hpp>

using namespace std::literals;
using asio::ip::udp;

TEST_CASE("network asio synchronous UDP", "[network]") {
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
		receivingSocket.receive(asio::buffer(bufferCopy));

		int diff = memcmp(buffer, bufferCopy, 4 * sizeof(int));
		REQUIRE(diff == 0);
	}
	catch (std::exception& e)
	{
		FAIL(e.what());
	}
}
