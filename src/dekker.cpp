#include <iostream>
#include <atomic>
#include <thread>

using namespace std;

const int COUNT = 10;

int main(int argc, char** argv)
{
	atomic_bool f1{false}, f2{false};
	
	auto proc1 = [&]() {
		for(int i = 0; i < COUNT;)
		{
			f1.store(true);
			if(f2.load() == false)
			{
				cout << "T1 in critical section" << endl;
				++i;
			}
			
			f1.store(false);
		}
	};
	
	auto proc2 = [&]() {
		for(int i = 0; i < COUNT;)
		{
			f2.store(true);
			if(f1.load() == false)
			{
				cout << "T2 in critical section" << endl;
				++i;
			}
			
			f2.store(false);
		}
	};
	
	thread t1(proc1);
	thread t2(proc2);
	
	t1.join();
	t2.join();
	
	return 1;
}
