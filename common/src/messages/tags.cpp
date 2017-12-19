
#include "messages/tags.hpp"
#include <cassert>
#include <unordered_map>


namespace sk::msg {

	namespace tag {

		family tag::family_of(tag_type tag) {
			assert(is_valid(tag) && "Invalid tag value");

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
				WRITE_PAIR(sync_homeroom),
				WRITE_PAIR(sync_instance),
				WRITE_PAIR(disconnect),
				WRITE_PAIR(heartbeat),
				WRITE_PAIR(login),
				WRITE_PAIR(change_world),
			};
			#undef WRITE_PAIR

			if (!is_valid(tag))
				return "invalid";

			return names[tag];
		}

	}

}