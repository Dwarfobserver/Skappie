
#pragma once

#include <vector>
#include "msg_tags.hpp"
#include "bytes_span.hpp"
#include "utils.hpp"


namespace sk {

	namespace msg { class wrapper; }
	bytes_span& operator<<(bytes_span& span, msg::wrapper const& wrapper);
	bytes_span& operator>>(bytes_span& span, msg::wrapper& wrapper);
	int serialized_size(msg::wrapper const& wrapper);

	namespace msg {

		/// Store any message type by serializing it in a owned buffer.
		class wrapper {
			friend bytes_span& sk::operator<<(bytes_span& span, wrapper const& wrapper);
			friend bytes_span& sk::operator>>(bytes_span& span, wrapper& wrapper);
			friend int sk::serialized_size(wrapper const& wrapper);
		public:
			wrapper() : tag_{ tag::invalid } {}

			template <class Message>
			wrapper(Message const& msg, bytes_span span);

			/// Assign the given message. A temporary buffer is needed, which must be greater
			/// than the serialized size of the message.
			template <class Message>
			void assign(Message const& msg, bytes_span span);

			/// Extract te message.
			template <class Message>
			void extract(Message& msg) const;

			/// Returns the tag of the stored message.
			tag_type tag() const { return tag_; }

			/// Returns the stamp of the stored message. It must be a request message.
			stamp_type stamp() const;

			/// Returns true if no message is stored by the wrapper.
			bool empty() const { return tag_ == tag::invalid; }

		private:
			tag_type tag_;
			std::vector<std::byte> bytes_;
		};

		template <class Message>
		wrapper::wrapper(Message const& msg, bytes_span span) {
			assign(msg, span);
		}

		template <class Message>
		void wrapper::assign(Message const& msg, bytes_span span) {
			const auto begin = span.begin;
			span << msg;
			SK_ASSERT(span.is_valid(),
				"Tried to wrap a message with a too small temporary buffer");
			tag_ = tag_of<Message>;
			bytes_.assign(begin, span.begin);
		}

		template <class Message>
		void wrapper::extract(Message& msg) const {
			SK_ASSERT(tag_ == tag_of<Message>,
				"Tried to extract msg of type " + tag::to_string(tag_of<Message>) +
				" but contains msg of type " + tag::to_string(tag_));

			bytes_span span{ bytes_.data() };
			span >> msg;
		}
	}

}
