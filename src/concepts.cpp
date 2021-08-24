#include <iostream>
#include <concepts>
#include <type_traits>

template<typename T> concept always_good = true;
template<typename T> concept always_good_2 = requires { requires true; };

template<typename T> requires always_good<T> void good(T) {} // ALWAYS compiles
template<typename T> requires always_good_2<T> void good_2(T) {} // ALWAYS compiles



template<typename T> concept always_bad = false;
template<typename T> concept always_bad_2 = requires { requires false; };

template<typename T> requires always_bad<T> void bad(T) {} // NEVER compiles
template<typename T> requires always_bad_2<T> void bad_2(T) {} // NEVER compiles



template<typename T> concept can_add = requires (T a)
{
	requires std::integral<T>;
	requires sizeof(T) >= sizeof(int);
	{ a + a } noexcept -> std::same_as<T>;
};

template<typename T> requires can_add<T> // ONE 'requires' needed
T add(T x, T y) noexcept { return x + y; }



template<typename T> concept can_sub = requires (T a)
{
	requires can_add<T>;
	{ a * -1 } noexcept -> std::same_as<T>;
	{ add<T>(a, a) } noexcept -> std::same_as<T>;
};

template<can_sub T> // NO 'requires' needed because 'can_sub' used instead of 'typename'
T sub(T x, T y) noexcept { return add(x, y * -1); }



template<typename T> concept can_add_and_sub = requires (T a)
{
	requires can_add<T> and can_sub<T>; // SECOND 'requires' needed
	{ sub<T>(a, a) } noexcept -> std::same_as<T>; // nothrow invocable AND returns type T, OR...
	requires std::is_nothrow_invocable_r_v<T, decltype(sub<T>), T, T>; // same as above
};

template<can_add_and_sub T>
T add_sub(T x, T y, T z) noexcept { return x + y - z; }



template<typename T> concept can_add_and_sub_2 =
	can_add<T> and can_sub<T> and
	std::is_nothrow_invocable_r_v<T, decltype(sub<T>), T, T>;

template<typename T> requires can_add_and_sub_2<T>
T sub_add(T x, T y, T z) noexcept { return x - y + z; }



template<typename T> requires std::integral<T> // ACCEPT any integral type
void foo(T t) { std::cout << "1st overload called with t = " << t << std::endl; }

template<std::integral T> requires(std::same_as<T, short> or sizeof(T) == 2) // ACCEPT only 'short' or 'sizeof eq 2'
void foo(T t) { std::cout << "2nd overload called with t = " << t << std::endl; }



int main()
{
	good(11); // ALWAYS GOOD because 'always_good' concept used
	good_2(11); // ALWAYS GOOD because 'always_good_2' concept used

	// bad(14); // ALWAYS ERROR because 'always_bad' concept used
	// bad_2(14); // ALWAYS ERROR because 'always_bad_2' concept used

	add(1, 2);
	// add<short>(1, 2); // ERROR because of 'requires sizeof(T) >= sizeof(int);' in 'can_add'
	sub(3, 4);
	// sub<const char*>("3", "4"); // ERROR because 'requires std::integral<T>;' in 'can_add'
	add_sub<long long>(5, 6, 7);
	sub_add<long long>(5, 6, 7);

	foo(17); // calls 1st overload
	foo(short{20}); // calls 2nd overload
}
