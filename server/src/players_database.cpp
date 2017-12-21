
#include "players_database.hpp"
#include <algorithm>
#include <iostream>


namespace sk {

	players_database::players_database(std::string const& path) :
		parser_{ file_parser::try_open(path) }
	{
		if (!parser_.is_open()) {
			parser_ = file_parser::try_create(path);

			file_line line;
			line.write_comment("Player database file");
			parser_.content().push_back(line);
			std::cout << "Create players database\n";
		}
		parser_.save_at_destruction(true);

		std::vector<std::string> vals;
		for (auto const& line : parser_.content()) {
			if (line.type() == file_line::empty ||
				line.type() == file_line::comment)
				continue;

			SK_ASSERT(line.type() == file_line::values
				&& "Bad file formatting");

			line.read_values(vals);
			SK_ASSERT(vals.size() == 2
				&& "Bad player info record");

			info newInfo;
			newInfo.nickname = vals[0];
			newInfo.password = vals[1];
			data_.emplace(std::move(vals[0]), std::move(newInfo));
		}
	}

	void players_database::save() {
		parser_.save();
	}

	bool players_database::try_remove(std::string const& nickname) {
		const auto itData = data_.find(nickname);
		if (itData == data_.end()) 
			return false;

		auto& lines = parser_.content();
		std::vector<std::string> vals;
		auto itFile = std::find_if(lines.begin(), lines.end(),
			[&vals, &nickname] (file_line const& line) {

			line.read_values(vals);
			if (line.type() == file_line::empty ||
				line.type() == file_line::comment)
				return false;
			return vals[0] == nickname;
		});
		SK_ASSERT(itFile != lines.end()
			&& "Data and file have different values");

		lines.erase(itFile);
		data_.erase(itData);
		return true;
	}

	bool players_database::try_register(info const& account) {
		const auto it = data_.find(account.nickname);
		if (it != data_.end())
			return false;
		
		file_line line;
		line.write_values({ account.nickname, account.password });
		parser_.content().push_back(std::move(line));
		data_.insert(it, std::make_pair(account.nickname, account));
		return true;
	}

	players_database::info const* players_database::try_get_infos(std::string const& nickname) const {
		const auto it = data_.find(nickname);
		if (it == data_.end())
			return nullptr;
		
		return &it->second;
	}
}