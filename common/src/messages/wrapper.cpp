
#include "messages/wrapper.hpp"


namespace sk {

	bytes_span& operator<<(bytes_span& span, msg::wrapper const& wrapper) {
		span << wrapper.tag_ << wrapper.bytes_;
		return span;
	}
	bytes_span& operator>>(bytes_span& span, msg::wrapper& wrapper) {
		span >> wrapper.tag_ >> wrapper.bytes_;
		return span;
	}

}
