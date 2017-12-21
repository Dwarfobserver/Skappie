
#include "network/udp_socket.hpp"
#include "network/packet.hpp"
#include "utils.hpp"

using udp = asio::ip::udp;

namespace sk::net {

	asio::ip::address localhost_ip = asio::ip::address::from_string("127.0.0.1");

	udp_socket::udp_socket() :
		ioService_{},
		socket_{ioService_}
	{}

	void udp_socket::bind(uint16_t port) {
		socket_.open(udp::v4());
		socket_.bind({ udp::v4() , port });
	}

	void udp_socket::unbind() {
		socket_.close();
	}

	void udp_socket::push(msg::wrapper const& wrapper, address_type const& addr) {
		auto& packets = packets_[addr];

		if (packets.empty() || !packets.back().try_push(wrapper)) {
			packets.emplace_back();
			packets.back().set_max_size(500);
			SK_ASSERT(packets.back().try_push(wrapper)
				&& "An empty packet did not have enough place for one message");
		}
	}

	void udp_socket::send_packets() {
		for (auto& pair : packets_) {
			auto const& addr = pair.first;
			auto& packets = pair.second;

			for (auto const& packet : packets) {
				bytes_span span{ buffer_, sizeof(buffer_) };
				span << packet;
				socket_.async_send_to(
					asio::buffer(buffer_, span.begin - buffer_),
					addr,
					[] (const asio::error_code& error, size_t bytesSend) {
					
					if (error) throw std::runtime_error{ error.message() };
				});
			}
		}
		ioService_.run();
		packets_.clear();
	}

	bool udp_socket::try_get_packet(packet& packet, address_type& addr) {
		if (socket_.available() == 0)
			return false;
		
		const auto size = socket_.receive_from(asio::buffer(buffer_), addr);
		bytes_span span{ buffer_, size };
		span >> packet;
		return true;
	}


}
