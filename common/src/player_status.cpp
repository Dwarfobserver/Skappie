
#include "player_status.hpp"
#include <unordered_map>
#include "utils.hpp"

namespace sk {
	
	namespace player_status {
		std::string to_string(player_status_type status) {

#define WRITE_PAIR(name) { name, #name }
			static std::unordered_map<player_status_type, std::string> names{
				WRITE_PAIR(unknown),
				WRITE_PAIR(connected),
			};
#undef WRITE_PAIR

			SK_ASSERT(names.size() == count,
				"Tag names were forgotten");

			if (status >= count)
				return "invalid";

			return names[status];
		}
	}

}
