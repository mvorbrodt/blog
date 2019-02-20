#include <iostream>
#include <atomic>
#include <thread>
#include <memory>
#include <cstring>
#include "queue.h"
using namespace std;

const int COUNT = 0;

int main(int argc, char** argv)
{
	struct UDT
	{
		byte data[16];
	};

	atomic<UDT> udt;
	cout << "atomic<UDT> is lock free = " << udt.is_lock_free() << endl;

	atomic_bool e1{false}, e2{false}, e3{false};
	assert(e1.is_lock_free());
	
	thread t1([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e1.compare_exchange_weak(e, false, memory_order_acq_rel) && !e) e = true;
			cout << "A" << endl;
			e2.store(true, memory_order_release);
		}
	});
	
	thread t2([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e2.compare_exchange_weak(e, false, memory_order_acq_rel)) e = true;
			cout << "B" << endl;
			e3.store(true, memory_order_release);
		}
	});
	
	thread t3([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e3.compare_exchange_weak(e, false, memory_order_acq_rel)) e = true;
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
