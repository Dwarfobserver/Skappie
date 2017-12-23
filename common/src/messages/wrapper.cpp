
#include "messages/wrapper.hpp"


namespace sk {

	bytes_span& operator<<(bytes_span& span, msg::wrapper const& wrapper) {
		const auto b = span.begin;
		span << wrapper.tag_ << wrapper.bytes_;

		SK_ASSERT(span.begin - b == serialized_size(wrapper),
			"Wrong serialized_size(wrapper) value, " +
			std::to_string(serialized_size(wrapper)) +
			" should be " + std::to_string(span.begin - b));

		return span;
	}
	bytes_span& operator>>(bytes_span& span, msg::wrapper& wrapper) {
		const auto b = span.begin;
		span >> wrapper.tag_ >> wrapper.bytes_;

		SK_ASSERT(span.begin - b == serialized_size(wrapper),
			"Wrong serialized_size(wrapper) value, " +
			std::to_string(serialized_size(wrapper)) +
			" should be " + std::to_string(span.begin - b));

		return span;
	}

	int serialized_size(msg::wrapper const& wrapper) {
		return sizeof(wrapper.tag_) + serialized_size(wrapper.bytes_);
	}

	msg::stamp_type msg::wrapper::stamp() const {
		SK_ASSERT(tag::family_of(tag_) == family::request,
			"Only request message family has a stamp (serialized first), "
			"but contains msg of type " + tag::to_string(tag_));

		return *reinterpret_cast<stamp_type const*>(bytes_.data());
	}

}
