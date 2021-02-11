#pragma once

#include <limits>
#include <type_traits>
#include <cmath>
#include <cstdint>

// solution 1
// can specify number of digits at run-time as the second parameter
// slowest due to 2 function calls
template<typename T>
requires std::is_floating_point_v<T>
auto my_round1(T v, unsigned char d)
{
	auto p = std::pow(T(10), T(d));
	return std::round(v * p) / p;
}

// sloution 2
// if used only with other constexpr the result will be evaluated
// entirely at compile time meaning no runtime cost :)

// recursive template to compute B^E at compile time
// result is stored as a static variable 'value' of type T
template<std::uint64_t B, unsigned char E, typename T>
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

// recursive function template to calculate b^e
// if both parameters are constexpr it will evaluate at compile time
// otherwise it will evaluate at run time
// returns the result as type T
template<typename T>
requires std::is_arithmetic_v<T>
constexpr T power_of_f(std::uint64_t b, unsigned char e)
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
template<unsigned char D, typename T>
requires std::is_floating_point_v<T>
constexpr auto my_round2(T v)
{
	/* option 1 */ //constexpr auto p = power_of_f<T>(10, D);
	/* option 2 */ constexpr auto p = power_of<10, D, T>::value;
	if(my_abs(v) > std::numeric_limits<T>::max() / p) return v; // v * p would overflow
	if(my_abs(v) * p > std::numeric_limits<std::int64_t>::max() - 1) return v; // v * p would not fit in int64_t
	return my_rnd(v * p) / p;
}

// PICK WHICH ONE TO USE HERE:
//template<unsigned char D, typename T> auto my_round(T v) { return my_round1(v, D); } // using solution 1
template<unsigned char D, typename T> constexpr auto my_round(T v) { return my_round2<D>(v); } // using solution 2
