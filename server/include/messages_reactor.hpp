
#pragma once

#include <functional>
#include <iostream>
#include "network/udp_socket.hpp"
#include "network/packet.hpp"
#include "players_database.hpp"
#include "player_info.hpp"
#include "globals.hpp"

namespace sk {
	
	class messages_reactor {
	public:
		messages_reactor();
		template <class Message>
		void set_callback(std::function<void(Message const&, player_info&)>&& callback);
		template <class RequestMsg, class ResponseMsg>
		void set_request_callback(std::function<ResponseMsg(RequestMsg const&, player_info&)>&& callback);
		void handle(msg::wrapper const& wrapper, player_info& player);
	private:
		std::byte buffer_[500];
		std::function<void(msg::wrapper const&, player_info&)> callbacks_[msg::tag::count];
	};

	template <class Message>
	void messages_reactor::set_callback(std::function<void(Message const&, player_info&)>&& callback) {
		if (callbacks_[msg::tag_of<Message>]) {
			std::cout << "WARNING: " << "Callback with tag "
				<< msg::tag::to_string(msg::tag_of<Message>) << " erased\n";
		}
		callbacks_[msg::tag_of<Message>] =
				[callback = std::move(callback)]
				(msg::wrapper const& wrapper, player_info& player) {
			Message message;
			wrapper.extract(message);
			callback(message, player);
		};
	}

	template <class RequestMsg, class ResponseMsg>
	void messages_reactor::set_request_callback(std::function<ResponseMsg(RequestMsg const&, player_info&)>&& callback) {
		static_assert(msg::tag_of<RequestMsg> == msg::tag_of<ResponseMsg>,
			"The request and the response must correspond");
		SK_ASSERT(msg::tag::family_of(msg::tag_of<RequestMsg>) == msg::family::request,
			"The request callback must be set only for request messages");

		std::function<void(RequestMsg const&, player_info&)> finalCallback =
				[this, callback = std::move(callback)]
				(RequestMsg const& message, player_info& player) {
			ResponseMsg response;
			if (player.try_get_response(message.stamp, response)) {
				bytes_span span{ buffer_, sizeof(buffer_) };
				gSocket().push(msg::wrapper{ response, span }, player.address);
			}
			else {
				response = callback(message, player);
				response.stamp = message.stamp;
				bytes_span span{ buffer_, sizeof(buffer_) };
				gSocket().push(msg::wrapper{ response, span }, player.address);
				player.store_response(message.stamp, response);
			}
		};
		set_callback(std::move(finalCallback));
	}

}
