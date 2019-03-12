#include <iostream>
#include <mutex>
#include <thread>
#include "event.hpp"
using namespace std;

mutex cout_lock;
#define trace(x) { scoped_lock<mutex> lock(cout_lock); cout << x << endl; }

const int COUNT = 3;

int main(int argc, char** argv)
{
	auto_event e1, e2;
	
	thread t1([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			e1.wait();
			trace("A");
			e2.signal();
		}
	});
	
	thread t2([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			e2.wait();
			trace("B");
			e1.signal();
		}
	});
	
	e1.signal();
	
	t1.join();
	t2.join();
	
	return 1;
}
