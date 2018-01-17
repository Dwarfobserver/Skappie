
#pragma once

#include <functional>
#include "messages/wrapper.hpp"
#include "network/udp_socket.hpp"
#include "globals.hpp"

namespace sk { class application_context; }

namespace sk::net {

	class request {
		friend class application_context;
	public:
		request();
		request(request&&) = delete;
	private:
		enum class status {
			running,
			resolved,
			timed_out
		};
		void update();
		bool handle(msg::wrapper const& message);

		msg::wrapper message_;
		address_type address_;
		msg::stamp_type stamp_;
		std::function<bool(msg::wrapper const&)> callback_;
		std::function<void()> callbackTimeout_;
		status status_;
		std::chrono::time_point<std::chrono::high_resolution_clock> firstSend_;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastSend_;
		bool gotResponse_;
	};

}
