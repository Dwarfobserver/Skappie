
#include "point.hpp"


namespace sk {

	float& point::operator[](int i) {
		return *reinterpret_cast<float*>(this + i);
	}
	float const& point::operator[](int i) const {
		return *reinterpret_cast<float const*>(this + i);
	}

	point point::operator+(point p) const {
		return point{ x + p.x, y + p.y };
	}
	point point::operator-(point p) const {
		return point{ x - p.x, y - p.y };
	}
	point point::operator*(float ratio) const {
		return point{ x * ratio, y * ratio };
	}
	point point::operator/(float ratio) const {
		return point{ x / ratio, y / ratio };
	}

	point& point::operator+=(point p) {
		x += p.x;
		y += p.y;
		return *this;
	}
	point& point::operator-=(point p) {
		x -= p.x;
		y -= p.y;
		return *this;
	}
	point& point::operator*=(float ratio) {
		x *= ratio;
		y *= ratio;
		return *this;
	}
	point& point::operator/=(float ratio) {
		x /= ratio;
		y /= ratio;
		return *this;
	}

	bytes_span& operator<<(bytes_span& span, point const& p) {
		return span << p.x << p.y;
	}
	bytes_span& operator>>(bytes_span& span, point& p) {
		return span >> p.x >> p.y;
	}

}
