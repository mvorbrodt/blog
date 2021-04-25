#include <iostream>
#include <mutex>
#include <thread>
#include "queue.hpp"

using namespace std;

const int COUNT = 10;

int main(int argc, char** argv)
{
	bounded_queue<int> q(5);
	mutex cout_lock;

	thread producer([&]() {
		for(int i = 1; i <= COUNT; ++i)
		{
			q.push(i);
			{
				scoped_lock<mutex> lock(cout_lock);
				cout << "push v = " << i << endl;
			}
		}
	});

	thread consumer([&]() {
		for(int i = 1; i <= COUNT; ++i)
		{
			this_thread::sleep_for(1s);
			int v;
			q.pop(v);
			{
				scoped_lock<mutex> lock(cout_lock);
				cout << "pop  v = " << v << endl;
			}
		}
	});

	producer.join();
	consumer.join();

	return 1;
}
