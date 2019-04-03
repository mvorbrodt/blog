#include <iostream>
#include <memory>

using namespace std;

struct bad
{
	bad() : p1(new int), p2(new int)
	{
		*p1 = 1;
		*p2 = 2;
	}

	~bad()
	{
		delete p1;
		delete p2;
	}

	int* p1;
	int* p2;
};

struct still_bad
{
	still_bad() try : p1(new int), p2(new int)
	{
		*p1 = 1;
		*p2 = 2;
	}
	catch(...)
	{
		if(p1) delete p1; // ILLEGAL~!!!
		if(p2) delete p2; // ILLEGAL~!!!
		/*
		 * From: https://en.cppreference.com/w/cpp/language/function-try-block
		 *
		 * The behavior is undefined if the catch-clause of a function-try-block used on a
		 * constructor or a destructor accesses a base or a non-static member of the object.
		 */
	}

	~still_bad()
	{
		delete p1;
		delete p2;
	}

	int* p1 = nullptr;
	int* p2 = nullptr;
};

struct good
{
	good() : p1(make_unique<int>()), p2(make_unique<int>())
	{
		*p1 = 1;
		*p2 = 2;
	}

	unique_ptr<int> p1;
	unique_ptr<int> p2;
};

int main()
{
	bad b;
	still_bad sb;
	good g;
}
