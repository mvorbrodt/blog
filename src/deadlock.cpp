#include <iostream>
#include <mutex>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>

using namespace std;
using namespace chrono;

void DO_SOME_WORK(const char* msg)
{
	{
		static mutex cout_lock;
		auto t = system_clock::to_time_t(system_clock::now());
		lock_guard guard(cout_lock);
		cout << msg << " @ " << ctime(&t);
	}
	this_thread::sleep_for(milliseconds(rand() % 10));
}

void BAD()
{
	mutex m1, m2;

	thread t1([&]()
	{
		while(true)
		{
			m1.lock();

			DO_SOME_WORK("Thread 1");

			m2.lock();

			DO_SOME_WORK("Thread 1");

			m1.unlock();
			m2.unlock();
		}
	});

	thread t2([&]()
	{
		while(true)
		{
			m2.lock();

			DO_SOME_WORK("Thread 2");

			m1.lock();

			DO_SOME_WORK("Thread 2");

			m1.unlock();
			m2.unlock();
		}
	});

	t1.join();
	t2.join();
}

void GOOD()
{
	mutex m1, m2;

	thread t1([&]()
	{
		while(true)
		{
			scoped_lock guard(m1, m2);

			DO_SOME_WORK("Thread 1");
		}
	});

	thread t2([&]()
	{
		while(true)
		{
			scoped_lock guard(m2, m1);

			DO_SOME_WORK("Thread 2");
		}
	});

	t1.join();
	t2.join();
}

int main()
{
	srand(time(NULL));
	//BAD();
	GOOD();
}
