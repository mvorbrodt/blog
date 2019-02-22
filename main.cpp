#include <iostream>
#include <atomic>
#include <thread>
using namespace std;

const int COUNT = 10;

int main(int argc, char** argv)
{
	atomic_bool f1{false}, f2{false};

	thread t1([&]() {
		for(int i = 0; i < COUNT;)
		{
			f1.store(true, memory_order_relaxed);
			if(f2.load(memory_order_seq_cst) == false)
			{
				cout << "T1 in critical section" << endl;
				f1.store(false, memory_order_relaxed);
				++i;
				this_thread::yield();
			}
			else
			{
				f1.store(false, memory_order_relaxed);
			}
		}
	});

	thread t2([&]() {
		for(int i = 0; i < COUNT;)
		{
			f2.store(true, memory_order_relaxed);
			if(f1.load(memory_order_seq_cst) == false)
			{
				cout << "T2 in critical section" << endl;
				f2.store(false, memory_order_relaxed);
				++i;
				this_thread::yield();
			}
			else
			{
				f2.store(false, memory_order_relaxed);
			}
		}
	});

	t1.join();
	t2.join();

	return 1;
}
