
#pragma once

#include <vector>
#include "player_data.hpp"
#include "missile_data.hpp"


namespace sk::world {
	
	struct scene_data {
		std::vector<player_data> players;
		std::vector<missile_data> missiles;
		uint16_t nextId;
	};

}
