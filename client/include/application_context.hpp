
#pragma once

#include "player_status.hpp"
#include "bytes_span.hpp"
#include "messages/msg_tags.hpp"
#include "network/udp_socket.hpp"
#include "network/request.hpp"
#include "globals.hpp"
#include <chrono>
#include <memory>
#include <functional>


namespace sk {

	class application_context {
	public:
		application_context();

		const bytes_span bufferSpan;
		const std::chrono::milliseconds msTimeout;
		const std::chrono::milliseconds msRetry;
		player_status_type status;

		void update();
		void handle_response(msg::wrapper const& response);

		std::chrono::milliseconds ping() const {  return msPing_; }
		void register_ping(std::chrono::milliseconds delay);

		template <class RequestMsg, class ResponseMsg>
		void make_request(
			RequestMsg message,
			net::address_type const& address,
			std::function<bool(ResponseMsg const&)>&& callback,
			std::function<void()>&& timedOut);
	private:
		std::byte buffer_[1'000];
		msg::stamp_type nextStamp_;
		std::chrono::milliseconds msPing_;
		std::vector<std::unique_ptr<net::request>> pRequests_;
	};

	template <class RequestMsg, class ResponseMsg>
	void application_context::make_request(
		RequestMsg message,
		net::address_type const& address,
		std::function<bool(ResponseMsg const&)>&& callback,
		std::function<void()>&& timedOut) {

		static_assert(msg::tag_of<RequestMsg> == msg::tag_of<ResponseMsg>);

		pRequests_.push_back(std::make_unique<net::request>());
		auto &request = *pRequests_.back();

		request.stamp_ = ++nextStamp_;
		message.stamp = request.stamp_;
		request.message_ = msg::wrapper{ message, gContext().bufferSpan };
		request.address_ = address;
		request.callback_ = [&request, callback = std::move(callback)]
		(msg::wrapper const& wrapper) -> bool {

			SK_ASSERT(wrapper.tag() == msg::tag_of<RequestMsg>,
				"Wrong message type passed to the request handler : "
				"tag = " + msg::tag::to_string(wrapper.tag()));
			ResponseMsg response;
			wrapper.extract(response);
			return callback(response);
		};
		request.callbackTimeout_ = std::move(timedOut);
		request.lastSend_ = std::chrono::high_resolution_clock::now();
		request.firstSend_ = request.lastSend_;
		gSocket().push(message, address);
	}


}
