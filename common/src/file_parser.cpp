
#include "file_parser.hpp"
#include <fstream>
#include <sstream>
#include "core.hpp"
#include <iostream>


namespace sk {

	static void assert_no_escape(std::string const& line) {
		SK_ASSERT(std::find(line.begin(), line.end(), '\n')
					== line.end()
			&& "Line string must not have '\\n'");
	}

	file_line::file_line() :
		type_(empty)
	{}

	void file_line::write_comment(std::string const& msg) {
		assert_no_escape(msg);
		content_ = ">" + msg;
		type_ = comment;
	}
	void file_line::read_comment(std::string& msg) const {
		SK_ASSERT(type_ == comment && "Bad file line read");
		msg.assign(content_.begin() + 1, content_.end());
	}

	void file_line::write_section(int val, std::string const& name) {
		assert_no_escape(name);
		SK_ASSERT(val >= 1 && val <= 9 && "Bad section value");
		content_ = '#' + std::to_string(val) + name;
		type_ = section;
	}
	void file_line::read_section(int& val, std::string& name) const {
		SK_ASSERT(type_ == section && "Bad file line read");
		std::istringstream ss{ content_ };
		char delim;
		ss >> delim >> val >> name;
	}

	void file_line::write_values(std::vector<std::string> const& vals) {
		std::ostringstream ss;
		for (auto const& str : vals) {
			assert_no_escape(str);
			ss << str << ';';
		}
		content_ = ss.str();
		type_ = values;
	}
	void file_line::read_values(std::vector<std::string>& vals) const {
		SK_ASSERT(type_ == values && "Bad file line read");
		std::istringstream ss{ content_ };
		vals.clear();
		vals.emplace_back();
		while (std::getline(ss, vals.back(), ';')) {
			std::cout << "STR = <" << vals.back() << ">\n";
			if (vals.back().empty()) continue;
			vals.emplace_back();
		}
	}

	file_line::file_line(std::string const& str) :
		content_{str}
	{
		assert_no_escape(str);
		if (str.empty())
			type_ = empty;
		else if (str[0] == '>')
			type_ = comment;
		else if (str[1] == '#')
			type_ = section;
		else
			type_ = values;
	}

	// File parser

	std::optional<file_parser> file_parser::try_open(std::string const& path) {
		std::ifstream file{ path };
		if (!file.is_open())
			return {};

		return {{ std::move(file), path }};
	}

	std::optional<file_parser> file_parser::try_create(std::string const& path) {
		std::fstream file{ path };
		if (file.is_open())
			return {};

		file.flush();
		file.close();

		return {{ std::ifstream{ path }, path }};
	}

	file_parser::~file_parser() {
		if (saveAtDestruction_) save();
	}

	file_parser::file_parser(std::ifstream&& file, std::string const& path) :
		path_{ path },
		saveAtDestruction_{ false }
	{
		std::string line;
		while (std::getline(file, line)) {
			content_.push_back(file_line{ line });
		}
	}

	void file_parser::save() {
		std::ofstream file{ path_ };
		file.clear();
		for (auto const& line : content_) {
			file << line.raw_string();
		}
	}
}
