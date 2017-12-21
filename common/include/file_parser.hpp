
#pragma once

#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include "utils.hpp"

namespace sk {

	class file_line {
		friend class file_parser;
	public:
		enum : int {
			comment,
			empty,
			section,
			values
		};
		file_line();

		bool operator==(file_line const& rhs) const;

		int type() const { return type_; }
		std::string const& raw_string() const { return content_; }

		void write_comment(std::string const& msg);
		void read_comment(std::string& msg) const;

		void write_section(int val, std::string const& name);
		void read_section(int& val, std::string& name) const;

		void write_values(std::vector<std::string> const& vals);
		void read_values(std::vector<std::string>& vals) const;
	private:
		explicit file_line(std::string const& str);

		std::string content_;
		int type_;
	};

	class file_parser {
	public:
		static file_parser try_open(std::string const& path);
		static file_parser try_create(std::string const& path);
		static bool try_remove(std::string const& path);

		file_parser();
		~file_parser();

		void save();
		void save(std::string const& path);

		std::vector<file_line>& content() { return content_; }

		bool is_open() const { return isOpen_; }
		std::string const& path() const { return path_; }

		bool save_at_destruction() const { return saveAtDestruction_; }
		void save_at_destruction(bool val) { saveAtDestruction_ = val; }
	private:
		file_parser(std::ifstream&& file, std::string const& path);

		bool isOpen_;
		std::vector<file_line> content_;
		std::string path_;
		bool saveAtDestruction_;
	};

}
