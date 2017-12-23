
#pragma once

#include <map>
#include <asio/ip/udp.hpp>
#include <asio/io_service.hpp>
#include "messages/wrapper.hpp"

namespace sk {

	namespace msg { class wrapper; }

	namespace net {

		class packet;

		extern asio::ip::address localhost_ip;
		using address_type = asio::ip::udp::endpoint;

		class udp_socket {
		public:
			udp_socket();
			void bind(uint16_t port);
			void unbind();
			template <class Message>
			void push(Message const& message, address_type const& addr);
			void push(msg::wrapper const& wrapper, address_type const& addr);
			void send_packets();
			bool try_get_packet(packet& packet, address_type& addr);
			uint16_t port() const;
		private:
			std::byte buffer_[500];
			asio::io_service ioService_;
			asio::ip::udp::socket socket_;
			std::map<address_type, std::vector<packet>> packets_;
		};

		template <class Message>
		void udp_socket::push(Message const& message, address_type const& addr) {
			push(msg::wrapper{ message, bytes_span{buffer_, sizeof(buffer_)} }, addr);
		}

	}
}
