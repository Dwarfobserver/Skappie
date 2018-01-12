
#include "messages/sync_players.hpp"
#include "bytes_span.hpp"

namespace sk {

	bytes_span& sk::operator<<(bytes_span& span, msg::player_data const& data) {
		return span << data.id << data.life << data.pos << data.speed;
	}
	bytes_span& sk::operator>>(bytes_span& span, msg::player_data& data) {
		return span >> data.id >> data.life >> data.pos >> data.speed;
	}

	bytes_span& sk::operator<<(bytes_span& span, msg::sync_players const& message) {
		span << message.timestamp;
		span << static_cast<uint16_t>(message.players.size());
		for (auto const& data : message.players) {
			span << data;
		}
		return span;
	}
	bytes_span& sk::operator>>(bytes_span& span, msg::sync_players& message) {
		span >> message.timestamp;
		uint16_t size;
		span >> size;
		message.players.resize(size);
		for (auto& data : message.players) {
			span >> data;
		}
		return span;
	}

}
