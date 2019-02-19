#include <iostream>
#include <atomic>
#include <thread>
using namespace std;

const int COUNT = 3;

int main(int argc, char** argv)
{
	atomic<bool> e1{false}, e2{false}, e3{false};
	
	thread t1([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e1.compare_exchange_strong(e, false, memory_order_acquire, memory_order_acquire)) e = true;
			cout << "A" << endl;
			e2.store(true, memory_order_release);
		}
	});
	
	thread t2([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e2.compare_exchange_strong(e, false, memory_order_acquire, memory_order_acquire)) e = true;
			cout << "B" << endl;
			e3.store(true, memory_order_release);
		}
	});
	
	thread t3([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e3.compare_exchange_strong(e, false, memory_order_acquire, memory_order_acquire)) e = true;
			cout << "C" << endl;
			e1.store(true, memory_order_release);
		}
	});
	
	e1.store(true);
	
	t1.join();
	t2.join();
	t3.join();
	
	return 1;
}
