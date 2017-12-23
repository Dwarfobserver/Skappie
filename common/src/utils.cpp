
#include "utils.hpp"


using namespace std::literals;

namespace sk {

	void detail::_assert(char const* expr, char const* file, int line, std::string const& msg) {
		auto str = "Skappie assert failed in :\n"s + file + "\nline " +
			std::to_string(line) + ", at " + expr + ".\nError message : " + msg;

		// Remove null-terminated characters
		for (auto& c : str) if (c == 0) c = '_';

		throw assert_exception(str);
	}

	assert_exception::assert_exception(std::string const& msg) :
		runtime_error{ msg }
	{}
}
