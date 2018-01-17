
#include "messages_reactor.hpp"
#include "globals.hpp"
#include "application_context.hpp"


namespace sk {
	messages_reactor::messages_reactor() {
		callbacks_[msg::tag::invalid] = [](msg::wrapper const& wrapper) {
			std::cout << "WARNING: message of tag "
				<< msg::tag::to_string(wrapper.tag())
				<< " and status "
				<< player_status::to_string(gContext().status)
				<< " has no callback\n";
		};
	}

	void messages_reactor::handle(msg::wrapper const& wrapper) {
		if (!msg::tag::is_valid(wrapper.tag())) {
			std::cout << "WARNING: tried to handle message with invalid tag\n";
			return;
		}
		auto& callback = callbacks_[gContext().status * msg::tag::count + wrapper.tag()];
		if (callback) callback(wrapper);
		else {
			auto& globalCallback = callbacks_[player_status::count * msg::tag::count + wrapper.tag()];
			if (globalCallback) globalCallback(wrapper);
			else callbacks_[msg::tag::invalid](wrapper);
		}
	}

}
