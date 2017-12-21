
#include "messages/disconnect.hpp"
#include "bytes_span.hpp"

namespace sk {

	bytes_span& sk::operator<<(bytes_span& span, msg::disconnect const& message) {
		return span;
	}

	bytes_span& sk::operator>>(bytes_span& span, msg::disconnect& message) {
		return span;
	}

}
