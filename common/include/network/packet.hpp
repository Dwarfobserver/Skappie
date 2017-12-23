
#pragma once

#include "bytes_span.hpp"


namespace sk {

	namespace net { class packet; }
	bytes_span& operator<<(bytes_span& span, net::packet const& packet);
	bytes_span& operator>>(bytes_span& span, net::packet& packet);
	int serialized_size(net::packet const& packet);

	namespace msg { class wrapper; }

	namespace net {

		class packet {
			friend bytes_span& sk::operator<<(bytes_span& span, packet const& packet);
			friend bytes_span& sk::operator>>(bytes_span& span, packet& packet);
			friend int sk::serialized_size(packet const& packet);
		public:
			packet();

			void clear();

			/// Returns the messages stored in the packet.
			std::vector<msg::wrapper> extract_messages() const;

			/// Returns true if the packet has no messages.
			bool empty() const { return header_.messagesCount == 0; }

			/// Returns the number of messages stored in the packet.
			int messages_count() const { return header_.messagesCount; }

			/// Set the max size that of the serialized packet.
			void set_max_size(int capacity);

			/// Try to serialize a message wrapper into the packet.
			bool try_push(msg::wrapper const& wrapper);
		private:
			struct header {
				uint32_t messagesCount;

				void in(bytes_span& span);
				void out(bytes_span& span) const;
			};
			header header_;
			bytes_span span_;
			std::vector<std::byte> body_;
		public:
			static constexpr int header_serialized_size = sizeof(header);
		};
	}

	std::string to_string(net::packet const& packet);

}
