
#include "world/algorithms.hpp"


namespace sk::world {

	slot_map<player_data>::key create_player(scene_data& scene, std::string const& name) {
		auto key = scene.players.emplace();
		auto& p = scene.players[key];
		p.name = name;
		p.data.id = scene.nextId++;
		p.data.life = player_data::max_life;
		return key;
	}

	slot_map<missile_data>::key create_missile(scene_data& scene, player_data const& caster, point direction) {
		auto key = scene.missiles.emplace();
		auto& m = scene.missiles[key];
		m.playerId = caster.data.id;
		m.pos = caster.data.pos + direction * missile_data::radius;
		m.speed = direction * missile_data::initial_speed;
		return key;
	}

	namespace {

		float distance(point p1, point p2) {
			const float x = p1.x - p2.x;
			const float y = p1.y - p2.y;
			return x * x + y * y;
		}

		void resolve_players_collisions(scene_data& scene, player_data& p) {
			const float radius = 4 * player_data::radius * player_data::radius;
			for (auto& p2 : scene.players) {
				if (&p == &p2) continue;
				const float dist = distance(p.data.pos, p2.data.pos);
				if (dist >= radius) continue;

				const float diff = (sqrt(radius) - sqrt(dist)) / 2;
				const float x = p.data.pos.x - p2.data.pos.x;
				const float y = p.data.pos.y - p2.data.pos.y;
				const float absX = abs(x);
				const float absY = abs(y);
				const float xRatio = absX / (absX + absY);
				const float diffX = diff * xRatio;
				const float diffY = diff * (1 - xRatio);

				if (x > 0) {
					p.data.pos.x -= diffX;
					p2.data.pos.x += diffX;
				}
				else {
					p.data.pos.x += diffX;
					p2.data.pos.x -= diffX;
				}
				if (y > 0) {
					p.data.pos.y -= diffY;
					p2.data.pos.y += diffY;
				}
				else {
					p.data.pos.y += diffY;
					p2.data.pos.y -= diffY;
				}
			}
		}

	}

	void update_scene(scene_data& scene, float secs) {
		for (auto& p : scene.players) {
			p.data.pos += p.data.speed * secs;
			resolve_players_collisions(scene, p);
		}
		for (auto& m : scene.missiles) {
			m.pos += m.speed * secs;
		}
	}

}
