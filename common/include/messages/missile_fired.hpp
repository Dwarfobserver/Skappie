
#pragma once

#include "msg_tags.hpp"
#include "point.hpp"


namespace sk {

	namespace msg {
		
		struct missile_fired {
			uint8_t timestamp;
			uint16_t playerId;
			point pos;
			point speed;
		};

	}

	struct bytes_span;

	bytes_span& operator<<(bytes_span& span, msg::missile_fired const& message);
	bytes_span& operator>>(bytes_span& span, msg::missile_fired& message);

}
