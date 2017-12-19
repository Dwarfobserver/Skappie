
#include "bytes_span.hpp"


namespace sk {

	bytes_span& operator<<(bytes_span& span, std::string const& data) {
		span << static_cast<uint32_t>(data.size());
		std::memcpy(span.begin, data.data(), data.size());
		span.begin += data.size();
		return span;
	}
	bytes_span& operator>>(bytes_span& span, std::string& data) {
		uint32_t size;
		span >> size;
		data.resize(size);
		std::memcpy(data.data(), span.begin, data.size());
		span.begin += data.size();
		return span;
	}

	bytes_span& operator<<(bytes_span& span, std::vector<std::byte> const& data) {
		span << static_cast<uint32_t>(data.size());
		std::memcpy(span.begin, data.data(), data.size());
		span.begin += data.size();
		return span;
	}
	bytes_span& operator>>(bytes_span& span, std::vector<std::byte>& data) {
		uint32_t size;
		span >> size;
		data.resize(size);
		std::memcpy(data.data(), span.begin, data.size());
		span.begin += data.size();
		return span;
	}

}
