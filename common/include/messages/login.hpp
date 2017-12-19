
#pragma once

#include "tags.hpp"


namespace sk {

	namespace msg {

		/// Data needed to make a login request.
		/// The stamp is automatically set.
		struct login_request {
			static constexpr tag_type tag = tag::login;
			stamp_type stamp;
			std::string nickname;
		};

		/// Data needed to respond to a login request.
		/// The stamp must match the request one.
		struct login_response {
			static constexpr tag_type tag = tag::login;
			stamp_type stamp;
			bool accepted;
			std::string reason;
		};

	}

	struct bytes_span;

	bytes_span& operator<<(bytes_span& span, msg::login_request const& request);
	bytes_span& operator>>(bytes_span& span, msg::login_request& request);

	bytes_span& operator<<(bytes_span& span, msg::login_response const& response);
	bytes_span& operator>>(bytes_span& span, msg::login_response& response);
}
