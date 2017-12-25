
#pragma once


namespace sk {

	class players_database;
	namespace net { class udp_socket; }

	namespace detail {
		extern players_database* pAccounts;
		extern net::udp_socket* pSocket;
	}
	
	void gInit();
	inline players_database& gAccounts() { return *detail::pAccounts; }
	inline net::udp_socket& gSocket()    { return *detail::pSocket; }
}
