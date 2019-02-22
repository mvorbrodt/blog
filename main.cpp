#include <iostream>
#include <atomic>
#include <thread>
using namespace std;

const int COUNT = 3;

int main(int argc, char** argv)
{
	atomic_bool f1{false}, f2{false};

	thread t1([&]() {
		for(int i = 0; i < COUNT;)
		{
			f1 = true;
			if(f2 == false)
			{
				cout << "T1 in critical section" << endl;
				f1 = false;
				++i;
			}
			else
			{
				f1 = false;
				this_thread::yield();
			}
		}
	});

	thread t2([&]() {
		for(int i = 0; i < COUNT;)
		{
			f2 = true;
			if(f1 == false)
			{
				cout << "T2 in critical section" << endl;
				f2 = false;
				++i;
			}
			else
			{
				f2 = false;
				this_thread::yield();
			}
		}
	});

	t1.join();
	t2.join();

	return 1;
}
