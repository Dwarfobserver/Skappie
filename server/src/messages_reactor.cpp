
#include "messages_reactor.hpp"
#include <iostream>

using namespace std::literals;

namespace sk {

	messages_reactor::messages_reactor() {
		callbacks_[msg::tag::invalid] = [] (msg::wrapper const& wrapper, player_info& player) {
			std::cout << "WARNING: message of tag "
				<< msg::tag::to_string(wrapper.tag())
				<< " received from " << player.address
				<< " has not callback callback\n";
		};
	}
	
	void messages_reactor::handle(msg::wrapper const& wrapper, player_info& player) {
		if (!msg::tag::is_valid(wrapper.tag())) {
			std::cout << "WARNING: tried to handle message with invalid tag\n";
			return;
		}
		auto& callback = callbacks_[wrapper.tag()];
		if (callback) callback(wrapper, player);
		else callbacks_[msg::tag::invalid](wrapper, player);
	}

}
