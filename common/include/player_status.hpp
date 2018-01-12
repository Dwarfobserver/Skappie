
#pragma once

#include <string>

namespace sk {
	
	using player_status_type = uint8_t;
	namespace player_status {
		enum type : uint8_t {
			unknown,
			connected,
			count
		};

		std::string to_string(player_status_type status);
	}

}
