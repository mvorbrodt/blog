#include <thread>
#include <cmath>
#include <boost/timer/timer.hpp>

using namespace std;
using namespace boost::timer;

int main()
{
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

	thread t1(procUser, 1000000000);
	thread t2(procSystem, 100000);

	t1.join();
	t2.join();

	return 0;
}
