#include <iostream>
#include <sstream>
#include "timer.h"
using namespace std;
using namespace chrono;

int main()
{
	auto start = high_resolution_clock::now();
	auto duration = [&]() {
		auto now = high_resolution_clock::now();
		auto msecs = duration_cast<milliseconds>(now - start).count();
		stringstream ss;
		ss << msecs / 1000.f;
		cout << "elapsed " << ss.str() << "s\t: ";
	};

	cout << "start" << endl;
	timer t(100ms);
	auto e1 = t.set_interval(10, [&]() { duration(); cout << "interval 1" << endl; });
	auto e2 = t.set_interval(20, [&]() { duration(); cout << "interval 2" << endl; });
	auto e3 = t.set_timeout(30, [&]() { duration(); cout << "timeout 1" << endl; });
	auto e4 = t.set_timeout(40, [&]() { duration(); cout << "timeout 2" << endl; });
	auto e5 = t.set_timeout(50, [&]() { duration(); cout << "timeout that never happens" << endl; });
	e5->signal(); // cancel this timeout
	this_thread::sleep_for(5s);
	cout << "pause intervals for 5s" << endl;
	e1->signal(); // cancel this interval
	e2->signal(); // cancel this interval
	this_thread::sleep_for(5s);
	cout << "resume intervals" << endl;
	e1->reset(); // resume this interval
	e2->reset(); // resume this interval
	this_thread::sleep_for(15s);
	cout << "end" << endl;
}
