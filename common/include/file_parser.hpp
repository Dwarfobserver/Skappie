
#pragma once

#include <optional>
#include <string>
#include <vector>
#include <sstream>
#include "core.hpp"

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
		static std::optional<file_parser> try_open(std::string const& path);
		static std::optional<file_parser> try_create(std::string const& path);

		~file_parser();
		void save();

		std::vector<file_line>& content() { return content_; }
		std::string const& path() const { return path_; }
		bool save_at_destruction() const { return saveAtDestruction_; }
		void save_at_destruction(bool val) { saveAtDestruction_ = val; }
	private:
		file_parser(std::ifstream&& file, std::string const& path);

		std::vector<file_line> content_;
		std::string path_;
		bool saveAtDestruction_;
	};

}
