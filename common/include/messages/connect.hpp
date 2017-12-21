
#pragma once

#include "tags.hpp"


namespace sk {

	namespace msg {

		/// Data needed to make a login request.
		/// The stamp is automatically set.
		struct connect_request {
			static constexpr tag_type tag = tag::connect;
			stamp_type stamp;
			std::string nickname;
		};

		/// Data needed to respond to a login request.
		/// The stamp must match the request one.
		struct connect_response {
			static constexpr tag_type tag = tag::connect;
			stamp_type stamp;
			bool accepted;
			std::string reason;
		};

	}

	struct bytes_span;

	bytes_span& operator<<(bytes_span& span, msg::connect_request const& request);
	bytes_span& operator>>(bytes_span& span, msg::connect_request& request);

	bytes_span& operator<<(bytes_span& span, msg::connect_response const& response);
	bytes_span& operator>>(bytes_span& span, msg::connect_response& response);
}
