
#include "globals.hpp"
#include "players_database.hpp"
#include "network/udp_socket.hpp"


namespace {
	struct globals_type {
		sk::players_database accounts;
		sk::net::udp_socket socket;
	};
}

namespace sk {

	namespace detail {
		players_database* pAccounts{ nullptr };
		net::udp_socket* pSocket{ nullptr };
	}

	void gInit() {
		static globals_type globals;
		detail::pAccounts = &globals.accounts;
		detail::pSocket = &globals.socket;
	}

}
