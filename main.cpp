#include <iostream>
#include <sstream>
#include "timer.h"
using namespace std;
using namespace chrono;

int main()
{
	auto start = high_resolution_clock::now();
	auto duration = [start]() {
		auto now = high_resolution_clock::now();
		auto msecs = duration_cast<milliseconds>(now - start).count();
		stringstream ss;
		ss << msecs / 1000.0;
		cout << "elapsed " << ss.str() << "s\t: ";
	};

	cout << "start" << endl;
	timer t(1ms);
	auto e1 = t.set_timeout(3s, [&]() { duration(); cout << "timeout 3s" << endl; });
	auto e2 = t.set_interval(1s, [&]() { duration(); cout << "interval 1s" << endl; });
	auto e3 = t.set_timeout(4s, [&]() { duration(); cout << "timeout 4s" << endl; });
	auto e4 = t.set_interval(2s, [&]() { duration(); cout << "interval 2s" << endl; });
	auto e5 = t.set_timeout(5s, [&]() { duration(); cout << "timeout that never happens" << endl; });
	e5->signal(); // cancel this timeout
	this_thread::sleep_for(5s);
	e2->signal(); // cancel this interval
	e4->signal(); // cancel this interval
	cout << "pause intervals for 5s" << endl;
	this_thread::sleep_for(5s);
	cout << "resume intervals" << endl;
	e2->reset(); // resume this interval
	e4->reset(); // resume this interval
	this_thread::sleep_for(5s);
	cout << "end" << endl;
}
