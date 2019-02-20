#pragma once

#include <iostream>
#include <string>

struct T
{
	T() noexcept { std::cout << "T()" << std::endl; }
	T(int i) noexcept : m_i(i) { std::cout << "T(" << m_i << ")" << std::endl; }
	T(const std::string& s) noexcept : m_i(0) { std::cout << "T(\"" << s << "\")" << std::endl; }
	T(const T& t) noexcept : m_i(t.m_i) { std::cout << "T(const T&)" << std::endl; }
	T(T&& t) noexcept : m_i(t.m_i) { std::cout << "T(&&)" << std::endl; }
	T& operator = (const T& rhs) noexcept { std::cout << "operator = (const T&)" << std::endl; m_i = rhs.m_i; return *this; }
	T& operator = (T&& rhs) noexcept { std::cout << "operator = (T&&)" << std::endl; m_i = rhs.m_i; return *this; }
	~T() noexcept { std::cout << "~T()" << std::endl; }

	operator int () const { std::cout << "T::operator int ()" << std::endl; return m_i; }

private:
	int m_i;
};

struct Q
{
	Q() noexcept {}
	Q(int i) noexcept : m_i(i) {}
	Q(const std::string& s) noexcept : m_i(0) {}
	Q(const Q& q) noexcept : m_i(q.m_i) {}
	Q(Q&& q) noexcept : m_i(q.m_i) {}
	Q& operator = (const Q& rhs) noexcept { m_i = rhs.m_i; return *this; }
	Q& operator = (Q&& rhs) noexcept { m_i = rhs.m_i; return *this; }
	~Q() noexcept { }
	
	operator int () const { return m_i; }
	
private:
	int m_i;
};
