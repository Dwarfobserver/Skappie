
#include "players_database.hpp"
#include <algorithm>
#include <iostream>
#include <winerror.h>


namespace sk {

	char const* players_database::path = "resources/players.db";

	players_database::players_database() :
		parser_{ file_parser::try_open(path) }
	{
		if (!parser_.is_open()) {
			parser_ = file_parser::try_create(path);

			file_line line;
			line.write_comment("Player accounts database");
			parser_.content().push_back(line);
			std::cout << "Create player accounts database\n";
		}
		parser_.save_at_destruction(true);

		std::vector<std::string> vals;
		for (auto const& line : parser_.content()) {
			if (line.type() == file_line::empty ||
				line.type() == file_line::comment)
				continue;

			SK_ASSERT(line.type() == file_line::values,
				"Bad file formatting");

			line.read_values(vals);
			SK_ASSERT(vals.size() == 2,
				"Bad player info record");

			player_account newInfo;
			newInfo.nickname = vals[0];
			newInfo.password = vals[1];
			data_.emplace(std::move(vals[0]), std::move(newInfo));
		}
	}

	void players_database::save() {
		parser_.save();
	}

	void players_database::remove_account(std::string const& nickname) {
		const auto itData = data_.find(nickname);
		SK_ASSERT(itData != data_.end(),
			"Tried to remove a non-existing account");

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
		SK_ASSERT(itFile != lines.end(),
			"Data and file have different values");

		lines.erase(itFile);
		data_.erase(itData);
	}

	void players_database::create_account(player_account const& account) {
		const auto it = data_.find(account.nickname);
		SK_ASSERT(it == data_.end(),
			"Tried to create an account with a taken nickname");
		
		file_line line;
		line.write_values({ account.nickname, account.password });
		parser_.content().push_back(std::move(line));
		data_.insert(it, std::make_pair(account.nickname, account));
	}

	player_account const* players_database::try_get_account(std::string const& nickname) const {
		const auto it = data_.find(nickname);
		if (it == data_.end())
			return nullptr;
		
		return &it->second;
	}
}
