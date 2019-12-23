#pragma once

#include <iostream>
#include <ostream>
#include <string>

struct T
{
	T() : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T()" << std::endl; }

	T(int x) : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(int x = " << x << ")" << std::endl; }

	T(const char* s) : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(const char* s = " << s << ")" << std::endl; }

	T(const std::string& s) : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(const std::string& s = " << s << ")" << std::endl; }

	T(std::string&& s) : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(std::string&& s = " << s << ")" << std::endl; }

	T(const T& t) : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::T(const T" << t.get_instance_number() << " &)" << std::endl; }

	T(T&& t) : instance_number(instance_counter++)
	{ std::cout << "T" << get_instance_number() << "::(T" << t.get_instance_number() << " &&)" << std::endl; }

	T& operator = (const T& rhs)
	{ std::cout << "T" << get_instance_number() << "::operator = (const T" << rhs.get_instance_number() << " &)" << std::endl; return *this; }

	T& operator = (T&& rhs)
	{ std::cout << "T" << get_instance_number() << "::operator = (T" << rhs.get_instance_number() << " &&)" << std::endl; return *this; }

	virtual ~T()
	{ std::cout << "T" << get_instance_number() << "::~T()" << std::endl; }

	virtual void foo() const
	{ std::cout << "T" << get_instance_number() << "::foo()" << std::endl; }

	int get_instance_number() const { return instance_number; }

private:
	inline static int instance_counter = 1;
	const int instance_number = 0;
};

struct Q : public T
{
	Q() : T()
	{ std::cout << "Q" << get_instance_number() << "::Q()" << std::endl; }

	Q(int x) : T(x)
	{ std::cout << "Q" << get_instance_number() << "::Q(int x = " << x << ")" << std::endl; }

	Q(const char* s)
	{ std::cout << "Q" << get_instance_number() << "::Q(const char* s = " << s << ")" << std::endl; }

	Q(const std::string& s)
	{ std::cout << "Q" << get_instance_number() << "::Q(const std::string& s = " << s << ")" << std::endl; }

	Q(std::string&& s)
	{ std::cout << "Q" << get_instance_number() << "::Q(std::string&& s = " << s << ")" << std::endl; }

	Q(const Q& q) : T(q)
	{ std::cout << "Q" << get_instance_number() << "::Q(const Q" << q.get_instance_number() << " &)" << std::endl; }

	Q(Q&& q) : T(q)
	{ std::cout << "Q" << get_instance_number() << "::Q(Q" << q.get_instance_number() << " &&)" << std::endl; }

	Q& operator = (const Q& rhs)
	{ std::cout << "Q" << get_instance_number() << "::operator = (const Q" << rhs.get_instance_number() << " &)" << std::endl; return *this; }

	Q& operator = (Q&& rhs)
	{ std::cout << "Q" << get_instance_number() << "::operator = (Q" << rhs.get_instance_number() << " &&)" << std::endl; return *this; }

	~Q()
	{ std::cout << "Q" << get_instance_number() << "::~Q()" << std::endl; }

	virtual void foo() const final override
	{ std::cout << "Q" << get_instance_number() << "::foo()" << std::endl; }

	void bar() const
	{ std::cout << "Q" << get_instance_number() << "::bar()" << std::endl; }
};

std::ostream& operator << (std::ostream& os, const T& t)
{
	os << "T" << t.get_instance_number();
	return os;
}

std::ostream& operator << (std::ostream& os, const Q& q)
{
	os << "Q" << q.get_instance_number();
	return os;
}
