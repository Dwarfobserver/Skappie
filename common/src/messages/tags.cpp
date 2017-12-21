
#include "messages/tags.hpp"
#include <unordered_map>
#include "utils.hpp"


namespace sk::msg {

	namespace tag {

		family tag::family_of(tag_type tag) {
			SK_ASSERT(is_valid(tag) && "Invalid tag value");

			if (tag < _state_sync_end)
				return family::state_sync;
			if (tag < _event_end)
				return family::event;

			return family::request;
		}

		std::string to_string(family familyValue) {
			static std::unordered_map<family, std::string> names{
				{ family::state_sync, "state_sync" },
				{ family::event, "event" },
				{ family::request, "client_request" },
			};

			if (names.find(familyValue) == names.end())
				return "invalid";

			return names[familyValue];
		}

		std::string to_string(tag_type tag) {

			#define WRITE_PAIR(name) { name, #name }
			static std::unordered_map<tag_type, std::string> names{
				WRITE_PAIR(sync_acq),
				WRITE_PAIR(sync_world),
				WRITE_PAIR(sync_events),
				WRITE_PAIR(disconnect),
				WRITE_PAIR(heartbeat),
				WRITE_PAIR(connect),
				WRITE_PAIR(load_world),
			};
			#undef WRITE_PAIR

			SK_ASSERT(names.size() == _tags_end - _tags_begin
				&& "Tag names were forgotten");

			if (!is_valid(tag))
				return "invalid";

			return names[tag];
		}

	}

}
