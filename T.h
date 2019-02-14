#pragma once

#include <iostream>
#include <string>

struct T
{
	T() noexcept { std::cout << "T()" << std::endl; }
	T(int i) noexcept { std::cout << "T(" << i << ")" << std::endl; }
	T(const std::string& s) noexcept { std::cout << "T(\"" << s << "\")" << std::endl; }
	T(const T&) noexcept { std::cout << "T(const T&)" << std::endl; }
	T(T&&) noexcept { std::cout << "T(&&)" << std::endl; }
	T& operator = (const T&) noexcept { std::cout << "operator = (const T&)" << std::endl; return *this; }
	T& operator = (T&&) noexcept { std::cout << "operator = (T&&)" << std::endl; return *this; }
	~T() noexcept { std::cout << "~T()" << std::endl; }
};
