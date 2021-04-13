#pragma once

#include <array>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <cmath>
#include <cstdint>

// solution 1.1
// can specify number of digits at run-time as the second parameter
// slowest due to 3 function calls
template<typename T>
requires std::is_floating_point_v<T>
auto runtime_round1(T v, unsigned d)
{
	auto p = std::pow(T(10), T(d));
	if(std::abs(v) > std::numeric_limits<T>::max() / p)
		return v; // v * p would overflow, do nothing...
	return std::round(v * p) / p;
}

// solution 1.2
// faster runtime solution, powers of 10 are compiled in, 2 function calls
// approach suggested by Ben from https://thoughts-on-coding.com
template<typename T>
requires std::is_floating_point_v<T>
auto runtime_round2(T v, unsigned d)
{
	static const auto k_p = std::array<T, 20>
	{
		 1e0,  1e1,  1e2,  1e3,  1e4,  1e5,  1e6,  1e7,  1e8,  1e9,
		1e10, 1e11, 1e12, 1e13, 1e14, 1e15, 1e16, 1e17, 1e18, 1e19
	};
	auto p = k_p.at(d);
	if(std::abs(v) > std::numeric_limits<T>::max() / p)
		return v; // v * p would overflow, do nothing...
	return std::round(v * p) / p;
}

// sloution 2
// if used only with other constexpr the result will be evaluated
// entirely at compile time meaning no runtime cost :)

// recursive template to compute B^E at compile time
// result is stored as a static variable 'value' of type T
template<std::uint64_t B, unsigned E, typename T>
requires std::is_arithmetic_v<T>
struct power_of
{
	static constexpr T value = T(B) * power_of<B, E - 1, T>::value;
};

// terminating template for the recursion one above once E == 0
template<std::uint64_t B, typename T>
requires std::is_arithmetic_v<T>
struct power_of<B, 0, T>
{
	static constexpr T value = T(1);
};

template<std::uint64_t B, unsigned E, typename T>
inline constexpr T power_of_v = power_of<B, E, T>::value;

// recursive function template to calculate b^e
// if both parameters are constexpr it will evaluate at compile time
// otherwise it will evaluate at run time
// returns the result as type T
template<typename T>
requires std::is_arithmetic_v<T>
constexpr T power_of_f(std::uint64_t b, unsigned e)
{
	return e == 0 ? T(1) : T(b) * power_of_f<T>(b, e - 1);
}

// given a value 'v' return +1 if v is >= 0, otherwise return -1
template<typename T>
requires std::is_arithmetic_v<T>
constexpr auto my_sign(T v)
{
	return v >= T(0) ? T(1) : T(-1);
}

// given a value 'v' return it's absolute value
template<typename T>
requires std::is_arithmetic_v<T>
constexpr auto my_abs(T v)
{
	return v >= T(0) ? v : -v;
}

// round float/double/long double value 'v' to the nearest integer
// using compile time type conversions
template<typename T>
requires std::is_floating_point_v<T>
constexpr auto my_rnd(T v)
{
	constexpr auto h = T(0.5) - std::numeric_limits<T>::epsilon();
	return (std::int64_t)(v + h * my_sign(v));
}

// self explanatory :)
// though number of digits must be provided at compile time
// as the first template parameter 'D'
template<unsigned D, typename T>
requires std::is_floating_point_v<T>
constexpr auto constexpr_round(T v)
{
	if(D >= std::numeric_limits<T>::digits10)
		return v; // 10 ^ D would not fit in int64_t, do nothing
	//constexpr auto p = power_of_f<T>(10, D);
	constexpr auto p = power_of_v<10, D, T>;
	if(my_abs(v) > std::numeric_limits<T>::max() / p)
		return v; // v * p would overflow, do nothing
	if(my_abs(v) * p > T(std::numeric_limits<std::int64_t>::max()))
		return v; // v * p would not fit in int64_t, do nothing
	return my_rnd(v * p) / p;
}
