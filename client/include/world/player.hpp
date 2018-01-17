
#pragma once

#include <SFML/Graphics/Sprite.hpp>
#include "world/player_data.hpp"
#include "slot_map.hpp"
#include "world/scene_data.hpp"


namespace sk::world {
	
	class player {
	public:
		player(scene_data& scene, std::string const& name);

		void update(float secs);
		void fire(point direction) const;
		sf::Sprite const& sprite() const { return sprite_; }
	private:
		sf::Sprite sprite_;
		slot_map<player_data>::key data_;
	};

}
