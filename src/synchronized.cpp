#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include "synchronized.hpp"

struct S
{
	void method() { std::cout << "non const\n"; }
	void method() const { std::cout << "const\n"; }
};

int main()
{
	shared_synchronized<S> s1;

	// takes exclusive lock
	s1->method();

	// takes shared lock
	std::as_const(s1)->method();

	// due to const'ness calling any method on s2, s3, or s4
	// will ALWAYS take shared lock; no need for std::as_const
	const shared_synchronized<S> s2;
	shared_synchronized<const S> s3;
	const shared_synchronized<const S> s4;

	// all 3 calls take shared lock
	s2->method();
	s3->method();
	s4->method();

	// std::vector shared by 3 threads t1, t2, and t3
	// t1 and t2 take exclusive locks, t3 takes shared lock
	shared_synchronized<std::vector<int>> sv;

	std::thread t1([&] ()
	{
		std::srand(std::time(NULL));
		while(true)
			// takes exclusive lock
			sv->push_back(rand());
	});

	std::thread t2([&] ()
	{
		while(true)
			// takes exclusive lock
			sv->clear();
	});

	std::thread t3([&] ()
	{
		while(true)
			// takes shared lock
			[[maybe_unused]] auto _ = std::as_const(sv)->empty();
	});

	t1.join();
	t2.join();
	t3.join();
}
