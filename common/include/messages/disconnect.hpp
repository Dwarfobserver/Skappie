
#pragma once

#include "msg_tags.hpp"


namespace sk {

	namespace msg {

		/// Data needed to make a connect request.
		/// The stamp is automatically set.
		/// The request can ask to login, to create an account or to remove one.
		struct disconnect {
			static constexpr tag_type tag = tag::disconnect;
		};

	}

	struct bytes_span;

	bytes_span& operator<<(bytes_span& span, msg::disconnect const& message);
	bytes_span& operator>>(bytes_span& span, msg::disconnect& message);

}
