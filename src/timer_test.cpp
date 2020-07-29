#include <iostream>
#include <sstream>
#include "timer.hpp"

using namespace std;
using namespace std::chrono;

int main()
{
	auto start = high_resolution_clock::now();
	auto duration = [start]()
	{
		auto now = high_resolution_clock::now();
		auto msecs = duration_cast<milliseconds>(now - start).count();
		stringstream ss;
		ss << msecs / 1000.0;
		cout << "elapsed " << ss.str() << "s\t: ";
	};

	timer t(10ms);

	auto t0 = t.set_timeout( 1ms, [&]() { duration(); cout << "start timeout" << endl; });
	auto t1 = t.set_timeout( 1s, [&]() { duration(); cout << "timeout  1s" << endl; });
	auto t2 = t.set_timeout( 2s, [&]() { duration(); cout << "timeout  2s" << endl; });
	auto t3 = t.set_timeout( 3s, [&]() { duration(); cout << "timeout  3s" << endl; });
	auto t4 = t.set_timeout( 5s, [&]() { duration(); cout << "timeout that never happens" << endl; });
	auto tf = t.set_timeout(10s, [&]() { duration(); cout << "end timeout" << endl; });

	auto i1 = t.set_interval(1s, [&]() { duration(); cout << "interval 1s" << endl; });
	auto i2 = t.set_interval(2s, [&]() { duration(); cout << "interval 2s" << endl; });
	auto i3 = t.set_interval(3s, [&]() { duration(); cout << "interval 3s" << endl; });
	auto i4 = t.set_interval(5s, [&]() { duration(); cout << "interval that never happens" << endl; });

	t0->wait();
	t4->cancel();
	i4->cancel();
	tf->wait();
}
