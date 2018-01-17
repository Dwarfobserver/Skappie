
#pragma once

#include "bytes_span.hpp"


namespace sk {
	
	struct alignas(float) point {
		float x;
		float y;

		float& operator[](int i);
		float const& operator[](int i) const;

		point operator+(point p) const;
		point operator-(point p) const;
		point operator*(float ratio) const;
		point operator/(float ratio) const;

		point& operator+=(point p);
		point& operator-=(point p);
		point& operator*=(float ratio);
		point& operator/=(float ratio);
	};

	bytes_span& operator<<(bytes_span& span, point const& p);
	bytes_span& operator>>(bytes_span& span, point& p);

}
