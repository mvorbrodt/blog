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
			while(!e1.compare_exchange_strong(e, false)) e = true;
			cout << "A" << endl;
			e2.store(true);
		}
	});
	
	thread t2([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e2.compare_exchange_strong(e, false)) e = true;
			cout << "B" << endl;
			e3.store(true);
		}
	});
	
	thread t3([&](){
		for(int i = 0; i < COUNT; ++i)
		{
			bool e = true;
			while(!e3.compare_exchange_strong(e, false)) e = true;
			cout << "C" << endl;
			e1.store(true);
		}
	});
	
	e1.store(true);
	
	t1.join();
	t2.join();
	t3.join();
	
	return 1;
}
