
#include "network/packet.hpp"
#include "messages/wrapper.hpp"
#include "utils.hpp"


namespace sk {

	bytes_span& sk::operator<<(bytes_span& span, net::packet const& packet) {
		SK_ASSERT(span.size() >= serialized_size(packet)
			&& "Span buffer too small");
		const auto b = span.begin;

		packet.header_.out(span);
		span << packet.body_;

		SK_ASSERT(span.begin - b == serialized_size(packet)
			&& "Bad serialization of the packet");
		return span;
	}
	bytes_span& sk::operator>>(bytes_span& span, net::packet& packet) {
		SK_ASSERT(span.size() >= serialized_size(packet)
			&& "Span buffer too small");
		const auto b = span.begin;

		packet.header_.in(span);
		span >> packet.body_;

		SK_ASSERT(span.begin - b == serialized_size(packet)
			&& "Bad de-serialization of the packet");
		return span;
	}

	int serialized_size(net::packet const& packet) {
		return net::packet::header_serialized_size + serialized_size(packet.body_);
	}

	namespace net {

		packet::packet() :
			header_{ 0 }
		{}

		void packet::clear() {
			header_.messagesCount = 0;
			body_.clear();
			span_.begin = body_.data();
		}

		std::vector<msg::wrapper> packet::extract_messages() const {
			std::vector<msg::wrapper> wrappers;
			wrappers.reserve(header_.messagesCount);

			bytes_span span{ body_.data(), body_.size() };
			while (span.size() > 0) {
				wrappers.emplace_back();
				span >> wrappers.back();
			}
			SK_ASSERT(span.size()  == 0
				&& wrappers.size() == header_.messagesCount
				&& "Bad de-serialization of the messages");

			return wrappers;
		}

		void packet::set_max_size(int capacity) {
			capacity -= header_serialized_size;
			capacity -= serialized_size(std::vector<std::byte>{});

			SK_ASSERT(capacity >= 0
				&& "The packet must be big enough for the header");

			const auto pos = span_.size();
			body_.reserve(capacity);
			span_ = { body_.data() + pos, body_.data() + capacity };
		}

		bool packet::try_push(msg::wrapper const& wrapper) {
			const int requiredSize = serialized_size(wrapper);
			if (span_.size() < requiredSize)
				return false;

			body_.resize(body_.size() + requiredSize);

			span_ << wrapper;
			++header_.messagesCount;

			return true;
		}

		void packet::header::in(bytes_span& span) {
			const auto b = span.begin;

			span >> messagesCount;

			SK_ASSERT(span.begin - b == header_serialized_size
				&& "Wrong header::serialized_size value");
		}
		void packet::header::out(bytes_span& span) const {
			const auto b = span.begin;

			span << messagesCount;

			SK_ASSERT(span.begin - b == header_serialized_size
				&& "Wrong header::serialized_size value");
		}
	}

}
