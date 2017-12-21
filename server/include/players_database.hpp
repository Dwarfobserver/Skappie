
#pragma once

#include "file_parser.hpp"
#include <map>


namespace sk {
	
	class players_database {
	public:
		struct info {
			std::string nickname;
			std::string password;
		};

		explicit players_database(std::string const& path);

		void save();

		bool try_remove(std::string const& nickname);
		bool try_register(info const& account);
		info const* try_get_infos(std::string const& nickname) const;
	private:
		file_parser parser_;
		std::map<std::string, info> data_;
	};

}
