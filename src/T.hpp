#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <atomic>
#include <utility>

struct T
{
	T() : instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T()" << std::endl; }

	T(int x) : m_x(x), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T(int x = " << T::x() << ")" << std::endl; }

	T(int x, int y, int z) : m_x(x), m_y(y), m_z(z), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T(int x = " << T::x() << ", int y = " << T::y() << ", int z = " << T::z() << ")" << std::endl; }

	T(const char* s) : m_s(s), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T(const char* s = \"" << T::s() << "\")" << std::endl; }

	T(const std::string& s) : m_s(s), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T(const std::string& s = \"" << T::s() << "\")" << std::endl; }

	T(std::string&& s) : m_s(std::move(s)), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T(std::string&& s = \"" << T::s() << "\")" << std::endl; }

	T(const T& t) : m_x(t.m_x), m_y(t.m_y), m_z(t.m_z), m_s(t.m_s), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::T(const T" << t.get_instance_number() << "/" << get_alive_number_at_construction() << " &)" << std::endl; }

	T(T&& t) noexcept : m_x(t.m_x), m_y(t.m_y), m_z(t.m_z), m_s(std::move(t.m_s)), instance_number(instance_counter++), alive_number(++alive_counter)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::(T" << t.get_instance_number() << "/" << get_alive_number_at_construction() << " &&)" << std::endl; }

	virtual ~T()
	{ std::cout << "T" << get_instance_number() << "/" << get_current_alive_number() << "::~T()" << std::endl; --alive_counter; }

	T& operator = (const T& rhs)
	{
		auto alive = get_current_alive_number();
		std::cout << "T" << get_instance_number() << "/" << alive << "::operator = (const T" << rhs.get_instance_number() << "/" << alive << " &)" << std::endl;
		m_x = rhs.m_x, m_y = rhs.m_y, m_z = rhs.m_z, m_s = rhs.m_s;
		return *this;
	}

	T& operator = (T&& rhs) noexcept
	{
		auto alive = get_current_alive_number();
		std::cout << "T" << get_instance_number() << "/" << alive << "::operator = (T" << rhs.get_instance_number() << "/" << alive << " &&)" << std::endl;
		m_x = rhs.m_x, m_y = rhs.m_y, m_z = rhs.m_z, m_s = std::move(rhs.m_s);
		return *this;
	}

	virtual void foo()
	{ std::cout << "T" << get_instance_number() << "/" << get_current_alive_number() << "::foo()" << std::endl; }

	virtual void bar() const
	{ std::cout << "T" << get_instance_number() << "/" << get_current_alive_number() << "::bar()" << std::endl; }

	int x() const { return(m_x); }
	int y() const { return(m_y); }
	int z() const { return(m_z); }
	const std::string& s() const { return(m_s); }

	int get_instance_number() const { return instance_number; }
	int get_alive_number_at_construction() const { return alive_number; }
	int get_current_alive_number() const { return alive_counter; }

private:
	int m_x = -1, m_y = -2, m_z = -3;
	std::string m_s = "empty";

	inline static std::atomic_int instance_counter = 1;
	inline static std::atomic_int alive_counter = 0;
	const int instance_number = -1;
	const int alive_number = 0;
};

struct Q final : public T
{
	Q() : T()
	{ std::cout << "Q" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q()" << std::endl; }

	Q(int x) : T(x)
	{ std::cout << "Q" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q(int x = " << T::x() << ")" << std::endl; }

	Q(int x, int y, int z) : T(x, y, z)
	{ std::cout << "T" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q(int x = " << T::x() << ", int y = " << T::y() << ", int z = " << T::z() << ")" << std::endl; }

	Q(const std::string& s) : T(s)
	{ std::cout << "Q" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q(const std::string& s = \"" << T::s() << "\")" << std::endl; }

	Q(std::string&& s) : T(std::forward<std::string>(s))
	{ std::cout << "Q" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q(std::string&& s = \"" << T::s() << "\")" << std::endl; }

	Q(const Q& q) : T(q)
	{ std::cout << "Q" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q(const Q" << q.get_instance_number() << "/" << get_alive_number_at_construction() << " &)" << std::endl; }

	Q(Q&& q) noexcept : T(std::forward<Q>(q))
	{ std::cout << "Q" << get_instance_number() << "/" << get_alive_number_at_construction() << "::Q(Q" << q.get_instance_number() << "/" << get_alive_number_at_construction() << " &&)" << std::endl; }

	virtual ~Q()
	{ std::cout << "Q" << get_instance_number() << "/" << get_current_alive_number() << "::~Q()" << std::endl; }

	Q& operator = (const Q& rhs)
	{
		auto alive = get_current_alive_number();
		std::cout << "Q" << get_instance_number() << "/" << alive << "::operator = (const Q" << rhs.get_instance_number() << "/" << alive << " &)" << std::endl;
		T::operator=(rhs);
		return *this;
	}

	Q& operator = (Q&& rhs) noexcept
	{
		auto alive = get_current_alive_number();
		std::cout << "Q" << get_instance_number() << "/" << alive << "::operator = (Q" << rhs.get_instance_number() << "/" << alive << " &&)" << std::endl;
		T::operator=(std::forward<Q>(rhs));
		return *this;
	}

	virtual void foo() final override
	{ std::cout << "Q" << get_instance_number() << "/" << get_current_alive_number() << "::foo()" << std::endl; }

	virtual void bar() const final override
	{ std::cout << "Q" << get_instance_number() << "/" << get_current_alive_number() << "::bar()" << std::endl; }
};

bool operator < (const T& lhs, const T& rhs)
{
	return lhs.x() < rhs.x();
}

std::ostream& operator << (std::ostream& os, const T& t)
{
	os << "T" << t.get_instance_number() << "/" << t.get_current_alive_number() << "(" << t.x() << ", " << t.y() << ", " << t.z() << ", \"" << t.s() << "\")";
	return os;
}

std::ostream& operator << (std::ostream& os, const Q& q)
{
	os << "Q" << q.get_instance_number() << "/" << q.get_current_alive_number() << "(" << q.x() << ", " << q.y() << ", " << q.z() << ", \"" << q.s() << "\")";
	return os;
}
