
#pragma once

#include "file_parser.hpp"
#include <map>


namespace sk {

	struct player_account {
		std::string nickname;
		std::string password;
	};
	
	class players_database {
	public:
		static char const* path;

		players_database();

		void save();

		void remove_account(std::string const& nickname);
		void create_account(player_account const& account);
		player_account const* try_get_account(std::string const& nickname) const;

		std::map<std::string, player_account> const& accounts() const { return data_; }
	private:
		file_parser parser_;
		std::map<std::string, player_account> data_;
	};

}
