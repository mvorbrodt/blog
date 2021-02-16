#define ENABLE_NEW_DELETE_TRACE_DUMP
#include "newtrace.hpp"

template<typename T, int N> void good()
{
	delete (new T);
	delete [] (new T [N]);

	struct Boom { Boom() { throw 1; } };

	try { new Boom; } catch(...) { } // not a leak!
	try { new Boom [N]; } catch(...) { } // same!
}

template<typename T, int N> void mismatch()
{
	delete [] (new T);
	delete (new T [N]);
}

template<typename T, int N> void leak()
{
	/*delete*/ new T;
	/*delete []*/ new T [N];
}

int main()
{
	good<char, 100>();
	mismatch<char, 100>();
	leak<char, 100>();
}
