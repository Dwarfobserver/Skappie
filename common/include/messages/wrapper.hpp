
#pragma once

#include "tags.hpp"
#include "bytes_span.hpp"
#include <vector>
#include <cassert>


namespace sk {

	namespace msg { class wrapper; }
	bytes_span& operator<<(bytes_span& span, msg::wrapper const& wrapper);
	bytes_span& operator>>(bytes_span& span, msg::wrapper& wrapper);

	namespace msg {

		class wrapper {
		public:
			wrapper() : tag_{ tag::invalid } {}

			tag_type tag() const { return tag_; }
			bool empty() const { return tag_ == tag::invalid; }

			template <class Message>
			void assign(Message const& msg, bytes_span span);

			template <class Message>
			void extract(Message& msg) const;
		private:
			tag_type tag_;
			std::vector<std::byte> bytes_;

			friend bytes_span& sk::operator<<(bytes_span& span, msg::wrapper const& wrapper);
			friend bytes_span& sk::operator>>(bytes_span& span, msg::wrapper& wrapper);
		};

		template <class Message>
		void wrapper::assign(Message const& msg, bytes_span span) {
			const auto begin = span.begin;
			span << msg;
			assert(span.is_valid() && "Tried to wrap a message with a too small temporary buffer");
			tag_ = tag_of<Message>;
			bytes_.assign(begin, span.begin);
		}

		template <class Message>
		void wrapper::extract(Message& msg) const {
			assert(tag_ == tag_of<Message> && "Tried to extracta wrong message type");
			bytes_span span{ bytes_.data() };
			span >> msg;
		}
	}

}
