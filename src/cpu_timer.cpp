#include <iostream>
#include <thread>
#include <cmath>
#include <boost/timer/timer.hpp>

using namespace std;
using namespace boost::timer;

int main()
{
	auto_cpu_timer program_timer(3);

	auto proc = [](long work)
	{
		cpu_timer thread_timer;
		thread_timer.start();

		for (long i = 0; i < work; ++i)
			sqrt(123.456L);

		thread_timer.stop();
		cout << "Thread timer:" << thread_timer.format(3);
	};

	thread t1(proc, 10000000);
	thread t2(proc, 100000000);
	thread t3(proc, 1000000000);

	t1.join();
	t2.join();
	t3.join();

	cout << "Program timer:";

	return 0;
}
