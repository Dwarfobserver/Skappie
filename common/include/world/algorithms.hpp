
#pragma once

#include "world/missile_data.hpp"
#include "world/player_data.hpp"
#include "world/scene_data.hpp"


namespace sk::world {
	
	player_data& create_player(scene_data& scene, std::string const& name);
	missile_data& create_missile(scene_data& scene, player_data const& caster, point direction);
	void update_scene(scene_data& scene, float secs);

}
