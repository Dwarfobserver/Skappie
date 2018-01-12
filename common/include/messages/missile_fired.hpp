
#pragma once

#include "msg_tags.hpp"


namespace sk {

	namespace msg {
		
		struct missile_fired {
			uint8_t timestamp;
			uint16_t playerId;
			float pos[2];
			float speed[2];
		};

	}

	struct bytes_span;

	bytes_span& operator<<(bytes_span& span, msg::missile_fired const& message);
	bytes_span& operator>>(bytes_span& span, msg::missile_fired& message);

}
