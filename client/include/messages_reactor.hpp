
#pragma once

#include <functional>
#include <iostream>
#include "player_status.hpp"
#include "messages/wrapper.hpp"


namespace sk {
	
	class messages_reactor {
	public:
		messages_reactor();
		void handle(msg::wrapper const& message);
		template <class Message>
		void set_callback(player_status_type status, std::function<void(Message const&)>&& callback);
		template <class Message>
		void set_callback(std::function<void(Message const&)>&& callback);
	private:
		std::byte buffer_[500];
		std::function<void(msg::wrapper const&)> callbacks_[(player_status::count + 1) * msg::tag::count];
	};

	template <class Message>
	void messages_reactor::set_callback(player_status_type status, std::function<void(Message const&)>&& callback) {
		const auto indice = status * msg::tag::count + msg::tag_of<Message>;
		if (callbacks_[indice]) {
			std::cout << "WARNING: " << "Callback with tag "
				<< msg::tag::to_string(msg::tag_of<Message>) << " for status "
				<< player_status::to_string(status) << " erased\n";

			callbacks_[indice] =
				[callback = std::move(callback)]
			(msg::wrapper const& wrapper) {
				Message message;
				wrapper.extract(message);
				callback(message);
			};
		}
	}

	template <class Message>
	void messages_reactor::set_callback(std::function<void(Message const&)>&& callback) {
		set_callback(player_status::count, std::move(callback));
	}

}
