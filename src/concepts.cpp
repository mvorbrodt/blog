#include <iostream>
#include <concepts>
#include <type_traits>

template<typename T> concept always_true = true;
template<typename T> concept always_true_2 = requires { requires true; };

template<typename T> requires always_true<T> void good(T) {} // ALWAYS compiles
template<typename T> requires always_true_2<T> void good_2(T) {} // ALWAYS compiles



template<typename T> concept always_false = false;
template<typename T> concept always_false_2 = requires { requires false; };

template<typename T> requires always_false<T> void bad(T) {} // NEVER compiles
template<typename T> requires always_false_2<T> void bad_2(T) {} // NEVER compiles



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



template<typename T> requires(std::integral<T> and sizeof(T) >= 4) // ACCEPT any integral type 32-bit or larger
void foo(T t) { std::cout << "1st foo overload called with t = " << t << std::endl; }

template<std::integral T> requires std::same_as<T, short> // ACCEPT only 'short'
void foo(T t) { std::cout << "2nd foo overload called with t = " << t << std::endl; }

void foo(auto t) requires std::same_as<decltype(t), char> // ACCEPT only 'char'
{ std::cout << "3rd foo overload called with t = " << (int)t << std::endl; }

void bar(std::integral auto t) // ACCEPT any integral type
{ std::cout << "1st bar overload called with t = " << t << std::endl; }

void bar(std::floating_point auto t) // ACCEPT any floating point type
{ std::cout << "2nd bar overload called with t = " << t << std::endl; }


#include <string>
template<typename T> auto xxx(T a, T b) { return a + b; }

int main()
{
	std::string s1("s1"), s2("s2");
	auto s3 = xxx(s1, s2);
	good(11); // ALWAYS GOOD because 'always_true' concept used
	good_2(11); // ALWAYS GOOD because 'always_true_2' concept used

	// bad(14); // ALWAYS ERROR because 'always_false' concept used
	// bad_2(14); // ALWAYS ERROR because 'always_false_2' concept used

	add(1, 2);
	// add<short>(1, 2); // ERROR because of 'requires sizeof(T) >= sizeof(int);' in 'can_add'
	sub(3, 4);
	// sub<const char*>("3", "4"); // ERROR because 'requires std::integral<T>;' in 'can_add'
	add_sub<long long>(5, 6, 7);
	sub_add<long long>(5, 6, 7);

	foo(11); // calls 1st overload
	foo(short{14}); // calls 2nd overload
	foo(char{17}); // calls 3rd overload

	bar(20); // calls 1st overload
	bar(23.f); // calls 2nd overload

	// RESTRICT local variable types to integral and floating point
	[[maybe_unused]] std::integral auto i = 11; // int
	[[maybe_unused]] std::floating_point auto f = 11.f; // float
	[[maybe_unused]] std::floating_point auto d = 11.; // double
	// std::integral auto i2 = 11.f; // ERROR because 'int' is expected
	// std::floating_point auto f2 = 11; // ERROR because 'float' is expected
}
