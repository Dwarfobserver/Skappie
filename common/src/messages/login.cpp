
#include "messages/login.hpp"
#include "bytes_span.hpp"

namespace sk {
	
	bytes_span& operator<<(bytes_span& span, msg::login_request const& request) {
		return span << request.stamp << request.nickname;
	}
	bytes_span& operator>>(bytes_span& span, msg::login_request& request) {
		return span >> request.stamp >> request.nickname;
	}

	bytes_span& operator<<(bytes_span& span, msg::login_response const& response) {
		return span << response.stamp << response.accepted << response.reason;
	}
	bytes_span& operator>>(bytes_span& span, msg::login_response& response) {
		return span >> response.stamp >> response.accepted >> response.reason;
	}

}
