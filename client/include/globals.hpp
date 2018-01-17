
#pragma once


namespace sk {

	class application_context;
	namespace net { class udp_socket; }

	namespace detail {
		extern application_context* pContext;
		extern net::udp_socket* pSocket;
	}

	void gInit();
	inline application_context& gContext() { return *detail::pContext; }
	inline net::udp_socket& gSocket()      { return *detail::pSocket; }

}
