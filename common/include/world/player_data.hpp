
#pragma once

#include "messages/sync_players.hpp"


namespace sk::world {
	
	struct player_data {
		constexpr static int radius = 60;
		constexpr static uint8_t max_life = 100;

		std::string name;
		msg::player_data data;
	};

}
