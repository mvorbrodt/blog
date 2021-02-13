#include <iostream>
#include <vector>

#define NEW_DELETE_TRACE_DUMP
#include "newtrace.hpp"

struct I { virtual ~I() {} };

template<typename T> struct S : I
{
	S(T* p, void(*d)(T*)) : ptr{ p }, del{ d } {}
	virtual ~S() { del(ptr); }

	T* ptr;
	void(*del)(T*);
};

using allocation_list_t = std::vector<I*>;
allocation_list_t my_allocations;

const int N = 5;

auto delete_one = [](auto* p) { delete p; };
auto delete_many = [](auto* p) { delete [] p; };

template<typename T> void good()
{
	my_allocations.push_back(new S<T>(new T, delete_one));
	my_allocations.push_back(new S<T>(new T[N], delete_many));
	for(auto s : my_allocations) delete s;
	my_allocations.clear();
}

template<typename T> void mismatch()
{
	my_allocations.push_back(new S<T>(new T, delete_many));
	my_allocations.push_back(new S<T>(new T[N], delete_one));
	for(auto s : my_allocations) delete s;
	my_allocations.clear();
}

template<typename T> void leak()
{
	my_allocations.push_back(new S<T>(new T, delete_many));
	my_allocations.push_back(new S<T>(new T[N], delete_one));
	//for(auto s : my_allocations) delete s;
	//my_allocations.clear();
}

int main()
{
	struct Boom { Boom() { throw 1; } };

	try { new Boom; } catch(...) { } // not a leak!
	try { new Boom [N]; } catch(...) { } // same!

	good<char>();
	mismatch<char>();
	leak<char>();
}
