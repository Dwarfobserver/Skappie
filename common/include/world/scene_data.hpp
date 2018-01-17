
#pragma once

#include "player_data.hpp"
#include "missile_data.hpp"
#include "slot_map.hpp"


namespace sk::world {
	
	struct scene_data {
		slot_map<player_data> players;
		slot_map<missile_data> missiles;
		uint16_t nextId;
	};

}
