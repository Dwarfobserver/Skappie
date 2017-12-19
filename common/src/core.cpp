
#include "core.hpp"


using namespace std::literals;

namespace sk {

	void detail::_assert(char const* expr, char const* file, int line) {
		throw assert_exception("Skappie assert failed in "s + file + " line "
			+ std::to_string(line) + ", at " + expr);
	}

	assert_exception::assert_exception(std::string const& msg) :
		runtime_error{ msg }
	{}
}
