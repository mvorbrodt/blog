#include <iostream>
#include <thread>
#include <cmath>
#include <cstdlib>
#include <boost/timer/timer.hpp>

using namespace std;
using namespace boost::timer;

int main()
{
	srand((unsigned int)time(NULL));

	auto_cpu_timer program_timer(3);

	auto procUser = [](long work)
	{
		for (long i = 0; i < work; ++i)
			sqrt(123.456L);
	};

	auto procSystem = [](long work)
	{
		for (long i = 0; i < work; ++i)
			thread([](){}).detach();
	};

	auto procTimer = [](long work)
	{
		cpu_timer timer;
		timer.start();

		for(long i = 0; i < work; ++i)
			rand();

		timer.stop();
		cout << "Thread timer:" << timer.format(3);
	};

	thread t1(procUser, 1000000000);
	thread t2(procSystem, 100000);
	thread t3(procTimer, 100000000);

	t1.join();
	t2.join();
	t3.join();

	cout << "Program timer:";

	return 1;
}
