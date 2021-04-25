#include <iostream>
#include <thread>
#include "queue.hpp"

int main()
{
	using namespace std;

	auto size = 15;
	auto delay = 100ms;
	bounded_queue<int> q( size );

	thread t{ [&]
	{
		while(true)
		{
			int i{};
			if(!q.pop(i))
				return;
			this_thread::sleep_for(delay);
			cout << "value: " << i << "\tsize: " << q.size() << endl;
		}
	}};

	for(int i = 1; i <= 50; ++i)
	{
		if(i % 2) q.push(i);
		else q.emplace(i);
	}

	this_thread::sleep_for((size + 1) * delay);
	q.unblock();
	t.join();
}
