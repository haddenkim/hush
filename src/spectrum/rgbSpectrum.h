#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
// #include <math/math.h>

template <typename T>
struct RGBSpectrum {
	/* constructors */
	RGBSpectrum<T>(T v = 0)
	{
		r = g = b = v;
	}
	RGBSpectrum<T>(T r, T g, T b)
		: r(r)
		, g(g)
		, b(b)
	{
		assert(!hasNaNs());
	}
	RGBSpectrum<T>(const RGBSpectrum<T>& s)
	{
		assert(!s.hasNaNs());
		r = s.r;
		g = s.g;
		b = s.b;
	}
	RGBSpectrum<T>& operator=(const RGBSpectrum<T>& s)
	{
		assert(!s.hasNaNs());
		r = s.r;
		g = s.g;
		b = s.b;
		return *this;
	}
	/* component access */
	float& operator[](int i)
	{
		assert(i >= 0 && i <= 2);
		if (i == 0)
			return r;
		if (i == 1)
			return g;
		return b;
	}
	float operator[](int i) const
	{
		assert(i >= 0 && i <= 2);
		if (i == 0)
			return r;
		if (i == 1)
			return g;
		return b;
	}
	/* unary operations */
	RGBSpectrum<T> operator-() const
	{
		return RGBSpectrum<T>(-r, -g, -b);
	}
	template <typename U>
	RGBSpectrum<T> operator*(U s) const
	{
		return RGBSpectrum<T>(s * r, s * g, s * b);
	}
	template <typename U>
	RGBSpectrum<T>& operator*=(U s)
	{
		assert(!std::isnan(s));
		r *= s;
		g *= s;
		b *= s;
		return *this;
	}

	template <typename U>
	RGBSpectrum<T> operator/(U s) const
	{
		assert(!std::isnan(s) || s != 0);
		return RGBSpectrum<T>(r / s, g / s, b / s);
	}
	template <typename U>
	RGBSpectrum<T>& operator/=(U s)
	{
		assert(!std::isnan(s) || s != 0);
		r /= s;
		g /= s;
		b /= s;
		return *this;
	}

	/* binary operations */
	RGBSpectrum<T> operator+(const RGBSpectrum<T>& s) const
	{
		assert(!s.hasNaNs());
		return RGBSpectrum<T>(r + s.r, g + s.g, b + s.b);
	}
	RGBSpectrum<T>& operator+=(const RGBSpectrum<T>& s)
	{
		assert(!s.hasNaNs());
		r += s.r;
		g += s.g;
		b += s.b;
		return *this;
	}
	RGBSpectrum<T> operator-(const RGBSpectrum<T>& s) const
	{
		assert(!s.hasNaNs());
		return RGBSpectrum<T>(r - s.r, g - s.g, b - s.b);
	}
	RGBSpectrum<T>& operator-=(const RGBSpectrum<T>& s)
	{
		assert(!s.hasNaNs());
		r -= s.r;
		g -= s.g;
		b -= s.b;
		return *this;
	}
	RGBSpectrum<T> operator*(const RGBSpectrum<T>& s) const
	{
		assert(!s.hasNaNs());
		return RGBSpectrum<T>(r * s.r, g * s.g, b * s.b);
	}
	RGBSpectrum<T>& operator*=(const RGBSpectrum<T>& s)
	{
		assert(!s.hasNaNs());
		r *= s.r;
		g *= s.g;
		b *= s.b;
		return *this;
	}
	RGBSpectrum<T> operator/(const RGBSpectrum<T>& s) const
	{
		assert(!s.hasNaNs());
		assert(s.r != 0 || s.g != 0 || s.b != 0);
		return RGBSpectrum<T>(r / s.r, g / s.g, b / s.b);
	}
	RGBSpectrum<T>& operator/=(const RGBSpectrum<T>& s)
	{
		assert(!s.hasNaNs());
		assert(s.r != 0 || s.g != 0 || s.b != 0);
		r /= s.r;
		g /= s.g;
		b /= s.b;
		return *this;
	}
	bool operator==(const RGBSpectrum<T>& s) const
	{
		return r == s.r && g == s.g && b == s.b;
	}
	bool operator!=(const RGBSpectrum<T>& s) const
	{
		return r != s.r || g != s.g || b != s.b;
	}

	/* data */
	T r, g, b;

	/* helpers */
	bool isBlack() const
	{
		return r == 0 && g == 0 && b == 0;
	}
	bool hasNaNs() const
	{
		return std::isnan(r) || std::isnan(g) || std::isnan(b);
	}
};

/* inline functions */
template <typename T, typename U>
inline RGBSpectrum<T> operator*(U a, const RGBSpectrum<T>& s)
{
	assert(!std::isnan(a) && !s.hasNaNs());
	return s * a;
}

template <typename T>
inline RGBSpectrum<T> Sqrt(const RGBSpectrum<T>& s)
{
	return RGBSpectrum<T>(std::sqrt(s.r), std::sqrt(s.g), std::sqrt(s.b));
}
template <typename T, typename U>
inline RGBSpectrum<T> Pow(const RGBSpectrum<T>& s, U e)
{
	return RGBSpectrum<T>(std::pow(s.r, e), std::pow(s.g, e), std::pow(s.b, e));
}
template <typename T>
inline RGBSpectrum<T> Exp(const RGBSpectrum<T>& s)
{
	return RGBSpectrum<T>(std::exp(s.r), std::exp(s.g), std::exp(s.b));
}

/* component-wise functions */
template <typename T>
T MinComponent(const RGBSpectrum<T>& s)
{
	return std::min(s.r, std::min(s.g, s.b));
}
template <typename T>
T MaxComponent(const RGBSpectrum<T>& s)
{
	return std::max(s.r, std::max(s.g, s.b));
}

/* linear interpolation */
template <typename T>
RGBSpectrum<T> Lerp(float t, const RGBSpectrum<T>& s0, const RGBSpectrum<T>& s1)
{
	return (1 - t) * s0 + t * s1;
}

/* clamp */
template <typename T>
RGBSpectrum<T> Clamp(const RGBSpectrum<T>& s, T low = 0, T high = std::numeric_limits<T>::infinity())
{
	return RGBSpectrum<T>(clamp(s.r, low, high), clamp(s.g, low, high), clamp(s.b, low, high));
}
