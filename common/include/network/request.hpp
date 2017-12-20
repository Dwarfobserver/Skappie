
#pragma once

#include <functional>
#include "messages/wrapper.hpp"
#include "network/context_info.hpp"


namespace sk::net {

	class request;
	template <class RequestMsg, class ResponseMsg>
	std::unique_ptr<request> make_request(
		context_info& context,
		RequestMsg const& message,
		address_type const& address,
		std::function<bool(ResponseMsg const&)>&& callback,
		std::function<void()>&& timedOut);

	class request {
		template <class RequestMsg, class ResponseMsg>
		friend std::unique_ptr<request> make_request(
			context_info& context,
			RequestMsg const& message,
			address_type const& address,
			std::function<bool(ResponseMsg const&)>&& callback,
			std::function<void()>&& timedOut);
	public:
		enum class status {
			running,
			resolved,
			timed_out
		};
		request(request&&) = delete;

		void update();
		bool handle(msg::wrapper const& message);
		
		msg::tag_type message_tag() const { return message_.tag(); }
		address_type const& address() const { return address_; }
		status get_status() const { return status_; }
	private:
		explicit request(context_info& context);

		context_info& context_;
		msg::wrapper message_;
		address_type address_;
		std::function<bool(msg::wrapper const&)> callback_;
		std::function<void()> callbackTimeout_;
		status status_;
		std::chrono::time_point<std::chrono::high_resolution_clock> firstSend_;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastSend_;
		bool gotResponse_;
	};

	template <class RequestMsg, class ResponseMsg>
	std::unique_ptr<request> make_request(
			context_info& context,
			RequestMsg const& message,
			address_type const& address,
			std::function<bool(ResponseMsg const&)>&& callback,
			std::function<void()>&& timedOut) {

		static_assert(msg::tag_of<RequestMsg> == msg::tag_of<ResponseMsg>);

		auto pRequest = std::make_unique<request>(context);
		pRequest->message_ = { message, { context.tmpBuffer, sizeof(context.tmpBuffer) } };
		pRequest->address_ = address;
		pRequest->callback_ = [pRequest = pRequest.get(), callback = std::move(callback)]
			(msg::wrapper const& wrapper) -> bool {

			SK_ASSERT(wrapper.tag() == msg::tag_of<RequestMsg>
				&& "Wrong message type passed to the request handler");
			ResponseMsg response;
			wrapper.extract(response);
			return callback(response);
		};
		pRequest->callbackTimeout_ = std::move(timedOut);
		context.pSocket->push(message, address);
		pRequest->lastSend_ = std::chrono::high_resolution_clock::now();
		pRequest->firstSend_ = pRequest->lastSend_;
		return pRequest;
	}

}
