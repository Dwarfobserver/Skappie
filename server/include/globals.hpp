
#pragma once

#include <map>
#include "network/udp_socket.hpp"


namespace sk {

	class players_database;
	class player_info;

	namespace detail {
		extern players_database* pAccounts;
		extern net::udp_socket* pSocket;
		extern std::map<net::address_type, player_info>* pPlayers;
	}
	
	void gInit();
	inline players_database& gAccounts() { return *detail::pAccounts; }
	inline net::udp_socket& gSocket()    { return *detail::pSocket; }
	inline std::map<net::address_type, player_info>& gPlayers() { return *detail::pPlayers; }
}
