
#include "globals.hpp"
#include "application_context.hpp"
#include "network/udp_socket.hpp"

namespace {
	struct globals_type {
		sk::application_context context;
		sk::net::udp_socket socket;

		globals_type() {
			context.status = sk::player_status::unknown;
		}
	};
}

namespace sk {

	namespace detail {
		application_context* pContext{ nullptr };
		net::udp_socket* pSocket{ nullptr };
	}

	void gInit() {
		static globals_type globals;
		detail::pContext = &globals.context;
		detail::pSocket = &globals.socket;
	}

}
