
#pragma once

#include "file_parser.hpp"
#include <map>


namespace sk {
	
	class players_database {
	public:
		static char const* path;

		struct info {
			std::string nickname;
			std::string password;
		};

		players_database();

		void save();

		bool try_remove(std::string const& nickname);
		bool try_register(info const& account);
		info const* try_get_infos(std::string const& nickname) const;

		std::map<std::string, info> const& data() const { return data_; }
	private:
		file_parser parser_;
		std::map<std::string, info> data_;
	};

}
