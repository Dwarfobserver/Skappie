
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>


namespace sk {

	/// Represents a non-owning buffer and provides flux operators to
	/// binary serialise data in and out of the buffer.
	struct bytes_span {
		std::byte* begin;
		std::byte* end;

		bytes_span() : begin{ nullptr }, end{ nullptr } {}

		bytes_span(void const* begin, void const* end) :
			begin(reinterpret_cast<std::byte*>(const_cast<void*>(begin))),
			end  (reinterpret_cast<std::byte*>(const_cast<void*>(end)))
		{}
		explicit bytes_span(void const* begin, int size = 0) :
			begin(reinterpret_cast<std::byte*>(const_cast<void*>(begin))),
			end(this->begin + size)
		{}
		explicit bytes_span(void const* begin, unsigned int size) :
			begin(reinterpret_cast<std::byte*>(const_cast<void*>(begin))),
			end(this->begin + size)
		{}

		/// Returns the gap between end and begin. Can be negative.
		int size() const { return static_cast<int>(end - begin); }

		/// Indicates if the end pointer is ahead of (or the same as) the begin pointer.
		/// Also, ensures that begin is different than nullptr.
		bool is_valid() const { return begin != nullptr && size() >= 0; }

		/// Extract data without advancing 'begin'.
		template <class T>
		bytes_span& peek(T& data);
	};

	std::string to_string(bytes_span span);

	template <class T>
	bytes_span& bytes_span::peek(T& data) {
		const auto b = begin;
		*this >> data;
		begin = b;
		return *this;
	}

	// serialized_size(data) functions are used to anticipate the size taken by overloads

	inline int serialized_size(std::string const& str) {
		return sizeof(uint32_t) + str.size();
	}
	inline int serialized_size(std::vector<std::byte> const& bytes) {
		return sizeof(uint32_t) + bytes.size();
	}

	// Convenient templates to implement basic types

	namespace detail {
		template <class T>
		bytes_span& out(bytes_span& span, T const& data) {
			*reinterpret_cast<T*>(span.begin) = data;
			span.begin += sizeof(T);
			return span;
		}
		template <class T>
		bytes_span& in(bytes_span& span, T& data) {
			data = *reinterpret_cast<T*>(span.begin);
			span.begin += sizeof(T);
			return span;
		}
	}

	// Basic types

	inline bytes_span& operator<<(bytes_span& span, bool const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, bool& data) {
		return detail::in(span, data);
	}

	inline bytes_span& operator<<(bytes_span& span, int8_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, int8_t& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, int16_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, int16_t& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, int32_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, int32_t& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, int64_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, int64_t& data) {
		return detail::in(span, data);
	}

	inline bytes_span& operator<<(bytes_span& span, uint8_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, uint8_t& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, uint16_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, uint16_t& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, uint32_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, uint32_t& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, uint64_t const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, uint64_t& data) {
		return detail::in(span, data);
	}

	inline bytes_span& operator<<(bytes_span& span, float const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, float& data) {
		return detail::in(span, data);
	}
	inline bytes_span& operator<<(bytes_span& span, double const& data) {
		return detail::out(span, data);
	}
	inline bytes_span& operator>>(bytes_span& span, double& data) {
		return detail::in(span, data);
	}

	// Overloads

	bytes_span& operator<<(bytes_span& span, std::string const& data);
	bytes_span& operator>>(bytes_span& span, std::string& data);

	bytes_span& operator<<(bytes_span& span, std::vector<std::byte> const& data);
	bytes_span& operator>>(bytes_span& span, std::vector<std::byte>& data);
	/*
	bytes_span& operator<<(bytes_span& span, bytes_span const& data);
	bytes_span& operator>>(bytes_span& span, bytes_span& data);
	*/
}
