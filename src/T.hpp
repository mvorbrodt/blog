#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <utility>

struct T
{
	T() : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T()" << std::endl; }

	T(int x) : m_x(x), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(int x = " << x << ")" << std::endl; }

	T(int x, int y, int z) : m_x(x), m_y(y), m_z(z), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(int x = " << x << ", int y = " << y << ", int z = " << z << ")" << std::endl; }

	T(const char* s) : m_s(s), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(const char* s = " << s << ")" << std::endl; }

	T(const std::string& s) : m_s(s), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(const std::string& s = " << s << ")" << std::endl; }

	T(std::string&& s) : m_s(std::move(s)), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(std::string&& s = " << s << ")" << std::endl; }

	T(const T& t) : m_x(t.m_x), m_y(t.m_y), m_z(t.m_z), m_s(t.m_s), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(const T" << t.get_instance_number() << " &)" << std::endl; }

	T(T&& t) noexcept : m_x(t.m_x), m_y(t.m_y), m_z(t.m_z), m_s(std::move(t.m_s)), instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::(T" << t.get_instance_number() << " &&)" << std::endl; }

	virtual ~T()
	{ std::cout << "T" << get_instance_number() << "::~T()" << std::endl; }

	T& operator = (const T& rhs)
	{
		std::cout << "T" << get_instance_number() << "::operator = (const T" << rhs.get_instance_number() << " &)" << std::endl;
		m_x = rhs.m_x, m_y = rhs.m_y, m_z = rhs.m_z, m_s = rhs.m_s;
		return *this;
	}

	T& operator = (T&& rhs) noexcept
	{
		std::cout << "T" << get_instance_number() << "::operator = (T" << rhs.get_instance_number() << " &&)" << std::endl;
		m_x = rhs.m_x, m_y = rhs.m_y, m_z = rhs.m_z, m_s = std::move(rhs.m_s);
		return *this;
	}

	virtual void foo()
	{ std::cout << "T" << get_instance_number() << "::foo()" << std::endl; }

	virtual void bar() const
	{ std::cout << "T" << get_instance_number() << "::bar()" << std::endl; }

	decltype(auto) x() const { return(m_x); }
	decltype(auto) y() const { return(m_y); }
	decltype(auto) z() const { return(m_z); }
	decltype(auto) s() const { return(m_s); }

	int get_instance_number() const { return instance_number; }

private:
	int m_x = -1, m_y = -2, m_z = -3;
	std::string m_s = "empty";

	inline static int instance_counter = 1;
	const int instance_number = -1;
};

struct Q final : public T
{
	Q() : T()
	{ std::cout << "Q" << get_instance_number() << "::Q()" << std::endl; }

	Q(int x) : T(x)
	{ std::cout << "Q" << get_instance_number() << "::Q(int x = " << x << ")" << std::endl; }

	Q(int x, int y, int z) : T(x, y, z)
	{ std::cout << "T" << get_instance_number() << "::Q(int x = " << x << ", int y = " << y << ", int z = " << z << ")" << std::endl; }

	Q(const char* s) : T(s)
	{ std::cout << "Q" << get_instance_number() << "::Q(const char* s = " << s << ")" << std::endl; }

	Q(const std::string& s) : T(s)
	{ std::cout << "Q" << get_instance_number() << "::Q(const std::string& s = " << s << ")" << std::endl; }

	Q(std::string&& s) : T(std::forward<std::string>(s))
	{ std::cout << "Q" << get_instance_number() << "::Q(std::string&& s = " << s << ")" << std::endl; }

	Q(const Q& q) : T(q)
	{ std::cout << "Q" << get_instance_number() << "::Q(const Q" << q.get_instance_number() << " &)" << std::endl; }

	Q(Q&& q) noexcept : T(std::forward<Q>(q))
	{ std::cout << "Q" << get_instance_number() << "::Q(Q" << q.get_instance_number() << " &&)" << std::endl; }

	virtual ~Q()
	{ std::cout << "Q" << get_instance_number() << "::~Q()" << std::endl; }

	Q& operator = (const Q& rhs)
	{
		std::cout << "Q" << get_instance_number() << "::operator = (const Q" << rhs.get_instance_number() << " &)" << std::endl;
		T::operator=(rhs);
		return *this;
	}

	Q& operator = (Q&& rhs) noexcept
	{
		std::cout << "Q" << get_instance_number() << "::operator = (Q" << rhs.get_instance_number() << " &&)" << std::endl;
		T::operator=(std::forward<Q>(rhs));
		return *this;
	}

	virtual void foo() final override
	{ std::cout << "Q" << get_instance_number() << "::foo()" << std::endl; }

	virtual void bar() const final override
	{ std::cout << "Q" << get_instance_number() << "::bar()" << std::endl; }
};

bool operator < (const T& lhs, const T& rhs)
{
	return lhs.x() < rhs.x();
}

std::ostream& operator << (std::ostream& os, const T& t)
{
	os << "T" << t.get_instance_number() << "(" << t.x() << ", " << t.y() << ", " << t.z() << ", \"" << t.s() << "\")";
	return os;
}

std::ostream& operator << (std::ostream& os, const Q& q)
{
	os << "Q" << q.get_instance_number() << "(" << q.x() << ", " << q.y() << ", " << q.z() << ", \"" << q.s() << "\")";
	return os;
}
