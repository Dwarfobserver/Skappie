
#pragma once

#include <string>
#include <sstream>


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

	namespace traits {

		namespace detail {
			template <template <class...> class Expression, typename SFINAE, typename... Ts>
			constexpr bool is_valid = false;

			template <template <class...> class Expression, typename... Ts>
			constexpr bool is_valid<Expression, std::void_t<Expression<Ts...>>, Ts...> = true;
		}

		template<template<class...> class Expression, typename... Ts>
		constexpr bool is_valid = detail::is_valid<Expression, void, Ts...>;
	}

	namespace detail {

		template<class T>
		using std_to_string_expr = decltype(std::to_string(std::declval<T>()));

		template<class T>
		using to_string_expr = decltype(to_string(std::declval<T>()));

		template<typename T>
		using sstream_expr = decltype(std::declval<std::ostringstream&>() << std::declval<T>());
	}

	namespace traits {

		template <class T>
		constexpr bool has_to_string = traits::is_valid<sk::detail::to_string_expr, T>;

		template <class T>
		constexpr bool has_std_to_string = traits::is_valid<sk::detail::std_to_string_expr, T>;

		template <class T>
		constexpr bool is_string_streamable = traits::is_valid<sk::detail::sstream_expr, T>;

		template <class T>
		constexpr bool can_make_string =
			std::is_constructible_v<std::string, T> ||
			traits::has_to_string<T> ||
			traits::has_std_to_string<T> ||
			traits::is_string_streamable<T>;
	}

	namespace detail {
		
		template <class T, class SFINAE = void>
		struct make_string_t {};

		template <class T>
		struct make_string_t<T, std::enable_if_t<
			std::is_constructible_v<std::string, T>
		>> {
			static std::string make_string(T const& val) {
				return std::string{ val };
			}
		};

		template <class T>
		struct make_string_t<T, std::enable_if_t<
				!std::is_constructible_v<std::string, T> &&
				traits::has_to_string<T>
		>> {
			static std::string make_string(T const& val) {
				return to_string(val);
			}
		};

		template <class T>
		struct make_string_t<T, std::enable_if_t<
			!std::is_constructible_v<std::string, T> &&
			!traits::has_to_string<T> &
			traits::has_std_to_string<T>
		>> {
			static std::string make_string(T const& val) {
				return std::to_string(val);
			}
		};

		template <class T>
		struct make_string_t<T, std::enable_if_t<
			!std::is_constructible_v<std::string, T> &&
			!traits::has_to_string<T> &
			!traits::has_std_to_string<T> &&
			traits::is_string_streamable<T>
		>> {
			static std::string make_string(T const& val) {
				std::ostringstream ss;
				ss << val;
				return ss.str();
			}
		};

	}

	template <class T>
	std::string make_string(T const& val) {
		return detail::make_string_t<T>::template make_string(val);
	}

}
