
#pragma once

#include <cstdint>
#include <string>


namespace sk::msg {

	/// The type of the stamp to identify request messages.
	using stamp_type = uint8_t;

	/// The type of the tag associated with each message type.
	using tag_type = uint8_t;

	/// Returns the tag associated to the message type.
	template <class Message>
	constexpr tag_type tag_of = Message::template tag;

	/// The families of message types.
	enum class family {
		/// Server's world descriptors, send in an unreliable way.
		state_sync,
		/// Events such as players inputs, send in an unreliable way.
		event,
		/// Requests made by clients, send in a reliable way.
		request
	};

	namespace tag {

		/// All tags available to categorize message types.
		enum : tag_type {
			invalid = 0,

			_tags_begin = 1,
			_state_sync_begin = _tags_begin,

				// state_sync family
				sync_homeroom = _state_sync_begin,
				sync_instance,

			_state_sync_end,
			_event_begin = _state_sync_end,

				// event family
				disconnect = _event_begin,

			_event_end,
			_request_begin = _event_end,

				// request family
				heartbeat = _request_begin,
				login,
				change_world,

			_request_end,
			_tags_end = _request_end
		};

		/// Returns true if the tag has a valid value.
		inline bool is_valid(tag_type tag) {
			return tag >= _tags_begin && tag < _tags_end;
		}

		/// Returns the family of the given valid tag.
		family family_of(tag_type tag);

		/// Returns the name of the given family value.
		std::string to_string(family familyValue);

		/// Returns the name of the given tag value.
		std::string to_string(tag_type tag);
	}


}
