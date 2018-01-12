
#include "globals.hpp"
#include "players_database.hpp"
#include "player_info.hpp"
#include "network/udp_socket.hpp"


namespace {
	struct globals_type {
		sk::players_database accounts;
		sk::net::udp_socket socket;
		std::map<sk::net::address_type, sk::player_info> players;
	};
}

namespace sk {

	namespace detail {
		players_database* pAccounts{ nullptr };
		net::udp_socket* pSocket{ nullptr };
		std::map<net::address_type, player_info>* pPlayers{ nullptr };
	}

	void gInit() {
		static globals_type globals;
		detail::pAccounts = &globals.accounts;
		detail::pSocket = &globals.socket;
		detail::pPlayers = &globals.players;
	}

}
