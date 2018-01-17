
#include "make_reactor.hpp"
#include "messages_reactor.hpp"
#include "messages.hpp"
#include "globals.hpp"
#include "application_context.hpp"

namespace sk {

	std::unique_ptr<messages_reactor> make_reactor() {
		auto pReactor = std::make_unique<messages_reactor>();

		std::function<void(msg::disconnect const&)> disconnectCallback =
			[] (msg::disconnect const& message) {
			
			gContext().status = player_status::unknown;
			std::cout << "Disconnected by the server\n";
		};
		pReactor->set_callback(player_status::connected, std::move(disconnectCallback));

		return pReactor;
	}

}
