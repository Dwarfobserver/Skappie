
#include "bytes_span.hpp"
#include "messages/missile_fired.hpp"


namespace sk {
	
	bytes_span& operator<<(bytes_span& span, msg::missile_fired const& message) {
		return span << message.timestamp << message.playerId << message.pos << message.speed;
	}

	bytes_span& operator>>(bytes_span& span, msg::missile_fired& message) {
		return span >> message.timestamp >> message.playerId >> message.pos >> message.speed;
	}

}
