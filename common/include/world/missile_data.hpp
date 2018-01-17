
#pragma once

#include "point.hpp"


namespace sk::world {

	struct missile_data {
		static constexpr int radius = 30;
		static constexpr int initial_speed = 200;

		uint16_t playerId;
		float lifetime;
		point pos;
		point speed;
	};

}
