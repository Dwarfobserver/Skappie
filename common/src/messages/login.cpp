
#include "messages/login.hpp"
#include "bytes_span.hpp"

namespace sk {
	
	bytes_span& operator<<(bytes_span& span, msg::login_request const& request) {
		span << request.stamp << request.nickname;
		return span;
	}
	bytes_span& operator>>(bytes_span& span, msg::login_request& request) {
		span >> request.stamp >> request.nickname;
		return span;
	}

	bytes_span& operator<<(bytes_span& span, msg::login_response const& response) {
		span << response.stamp << response.accepted << response.reason;
		return span;
	}
	bytes_span& operator>>(bytes_span& span, msg::login_response& response) {
		span >> response.stamp >> response.accepted >> response.reason;
		return span;
	}

}
