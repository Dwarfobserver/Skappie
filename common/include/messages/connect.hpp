
#pragma once

#include "msg_tags.hpp"


namespace sk {

	namespace msg {

		/// Data needed to make a connect request.
		/// The stamp is automatically set.
		/// The request can ask to login, to create an account or to remove one.
		struct connect_request {
			enum : uint8_t {
				login,
				new_account,
				remove_account
			};
			static constexpr tag_type tag = tag::connect;
			stamp_type stamp;
			uint8_t type;
			std::string nickname;
			std::string password;
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
