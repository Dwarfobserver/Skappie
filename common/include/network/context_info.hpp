
#pragma once
#include <chrono>
#include "udp_socket.hpp"


namespace sk::net {
	
	/// Informations shared to netork components.
	struct context_info {
		static context_info default_info();

		std::byte tmpBuffer[1'000];
		std::chrono::milliseconds ping;
		std::chrono::milliseconds requestRetry;
		std::chrono::milliseconds timeout;
		address_type serverAddress;
		udp_socket* pSocket;
		int packetSize;

		std::pair<bool, char const*> is_valid() const;
		void register_ping(std::chrono::milliseconds delay);

		static const float pingIntegrator;
	};

}
