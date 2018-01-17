
#pragma once

#include "msg_tags.hpp"
#include "point.hpp"
#include <vector>


namespace sk {

	namespace msg {

		struct player_data {
			uint16_t id;
			uint8_t life;
			point pos;
			point speed;
		};

		struct sync_players {
			static constexpr tag_type tag = tag::disconnect;

			uint8_t timestamp;
			std::vector<player_data> players;
		};

	}

	struct bytes_span;

	bytes_span& operator<<(bytes_span& span, msg::player_data const& data);
	bytes_span& operator>>(bytes_span& span, msg::player_data& data);

	bytes_span& operator<<(bytes_span& span, msg::sync_players const& message);
	bytes_span& operator>>(bytes_span& span, msg::sync_players& message);

}

