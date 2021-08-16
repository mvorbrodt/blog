#include <iostream>
#include <thread>
#include <vector>
#include <cstdlib>
#include "synchronized.hpp"

int main()
{
	using namespace std;

	srand(time(NULL));
	synchronized<vector<int>> sv;
	//auto sv = new vector<int>;

	thread t1([&] ()
	{
		while(true)
			sv->push_back(rand());
	});

	thread t2([&] ()
	{
		while(true)
			sv->clear();
	});

	t1.join();
	t2.join();
}
