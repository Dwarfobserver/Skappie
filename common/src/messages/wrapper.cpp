
#include "messages/wrapper.hpp"


namespace sk {

	bytes_span& operator<<(bytes_span& span, msg::wrapper const& wrapper) {
		const auto b = span.begin;
		span << wrapper.tag_ << wrapper.bytes_;
		SK_ASSERT(span.begin - b == serialized_size(wrapper)
			&& "Wrong serialized_size(wrapper) value");
		return span;
	}
	bytes_span& operator>>(bytes_span& span, msg::wrapper& wrapper) {
		const auto b = span.begin;
		span >> wrapper.tag_ >> wrapper.bytes_;
		SK_ASSERT(span.begin - b == serialized_size(wrapper)
			&& "Wrong serialized_size(wrapper) value");
		return span;
	}

	int serialized_size(msg::wrapper const& wrapper) {
		return sizeof(wrapper.tag_) + serialized_size(wrapper.bytes_);
	}

}
