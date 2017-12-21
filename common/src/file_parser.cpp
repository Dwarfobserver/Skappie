
#include "file_parser.hpp"
#include <fstream>
#include <sstream>
#include <cstdio>
#include "utils.hpp"


namespace sk {

	static void assert_no_escape(std::string const& line) {
		SK_ASSERT(std::find(line.begin(), line.end(), '\n')
					== line.end()
			&& "Line string must not have '\\n'");
	}

	file_line::file_line() :
		type_(empty)
	{}

	bool file_line::operator==(file_line const& rhs) const {
		return content_ == rhs.content_;
	}

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
			vals.emplace_back();
		}
		vals.pop_back();
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

	file_parser file_parser::try_open(std::string const& path) {
		std::ifstream file{ path };
		if (!file.is_open())
			return {};

		return { std::move(file), path};
	}

	file_parser file_parser::try_create(std::string const& path) {
		std::fstream file{ path };
		if (file.is_open())
			return {};

		file.flush();
		file.close();

		return { std::ifstream{ path }, path };
	}

	bool file_parser::try_remove(std::string const& path) {
		return std::remove(path.c_str()) == 0;
	}

	file_parser::file_parser() :
		isOpen_{ false },
		saveAtDestruction_{ false }
	{}

	file_parser::~file_parser() {
		if (saveAtDestruction_) save();
	}

	void file_parser::save() {
		SK_ASSERT(isOpen_
			&& "Nothing to save");
		save(path_);
	}

	file_parser::file_parser(std::ifstream&& file, std::string const& path) :
		isOpen_{ true },
		path_{ path },
		saveAtDestruction_{ false }
	{
		std::string line;
		while (std::getline(file, line)) {
			content_.push_back(file_line{ line });
		}
	}

	void file_parser::save(std::string const& path) {
		std::ofstream file{ path };
		file.clear();
		if (content_.empty()) return;

		file << content_.front().raw_string();
		for (int i = 1; i < content_.size(); ++i) {
			file << '\n' << content_[i].raw_string();
		}
	}
}
