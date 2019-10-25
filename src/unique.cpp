#include <iostream>
#include <memory>
#include <vector>
#ifdef NDEBUG
#undef NDEBUG
#include <cassert>
#define NDEBUG
#endif
#include "T.hpp"

using namespace std;

using T_u_ptr = unique_ptr<T>;
using T_s_ptr = shared_ptr<T>;

// Creates and gives away explicit ownership...
T_u_ptr source() { return make_unique<T>(); }

// Assumes explicit ownership, "t" gets deallocated at the end of this function
void sink(T_u_ptr t) { t->foo(); }

// Does NOT assume explicit ownership because we're taking by reference
void NOT_sink(T_u_ptr& t) { t->foo(); }

// Assumes ownership, but then hands it back... if the caller captures the return value,
// otherwise releases the resource
T_u_ptr sink_or_pass_thru(T_u_ptr t) { t->foo(); return t; }

// Just a function that takes a shared_ptr...
void shared(T_s_ptr t) { t->foo(); }

int main()
{
	auto t1 = source();
	sink(move(t1)); // We have to std::move it, because copy-constructor of unique_ptr = delete,
	                // by using std::move we're forcing the use of the move constructor (if one exists),
	                // this would have worked without std::move if using std::auto_ptr (now deprecate4d)
	                // and it would have stole the ownership without warning us!!!
	assert(!t1);    // "t1" is now pointing to null because of the std::move above

	auto t2 = source();
	NOT_sink(t2);   // "t2" still pointing to resource after this call
	assert(t2);
	sink(move(t2)); // and now "t2" is gone...
	assert(!t2);

	sink(source()); // No need for explicit std::move, temporary is captured as r-value reference
	                // so the unique_ptr's move constructor is automatically invoked

	auto t3 = source();
	auto t4 = sink_or_pass_thru(move(t3)); // Effectively moves the ownership from "t3" to "t4"
	assert(!t3 && t4);

	sink_or_pass_thru(source()); // Takes ownership, but deletes the resource since nobody captures the return value

	T_s_ptr t5 = source(); // Create and "upgrade" from unique to shared ownership
	T_s_ptr t6 = move(t4); // unique_ptr's must be explicitly std::move'ed to shared_ptr's
	assert(!t4 && t5 && t6);

	shared(t6); // No transfer of ownership, just using a shared resource here...

	// PRIMITIVE ARRAYS...

	constexpr const int N = 3;

	auto a1 = make_unique<int[]>(N);   // Allocates N int's, size of array is lost, values are undefined
	auto a2 = vector<int>(N, int{42}); // Allocates and value-initializes N int's, size is known, values are well defined
	auto a3 = move(a1);                // Transfer ownership of from "a1" to "a3"
	assert(!a1 && a3);

	a3[N - 1] = 1; // Access the last int of the array

	// ARRAYS...

	auto a4 = make_unique<T[]>(N); // Create an array of N T's, size is lost, T must have a default constructor
	auto a5 = vector<T>(N, T{42}); // Create a vector of N T's, size is known, initialize with custom T
	auto a6 = move(a4);            // Transfer ownership from "a4" to "a6"
	assert(!a4 && a6);

	a6[N - 1].foo(); // Access the last T of the array
}
