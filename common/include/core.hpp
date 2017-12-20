
#pragma once

#include <string>


namespace sk {

	class assert_exception : public std::runtime_error {
	public:
		explicit assert_exception(std::string const& msg);
	};

	namespace detail {
		void _assert(char const* expr, char const* file, int line);
	}

#ifdef NDEBUG
	#define SK_ASSERT(expression) ((void)0)
#else
	#define SK_ASSERT(expression) (void) (                                 \
				(!!(expression)) ||                                         \
				(detail::_assert(#expression, __FILE__, (int)(__LINE__)), 0) \
			)
#endif

}
