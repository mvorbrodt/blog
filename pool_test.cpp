#include <iostream>
#include <chrono>
#include <cstdlib>
#include "pool.hpp"
using namespace std;
using namespace chrono;

const int COUNT = 1'000'000;
const int REPS = 10;

int main()
{
	srand(0);
	auto start = high_resolution_clock::now();
	{
		simple_thread_pool tp;
		for(int i = 0; i < COUNT; ++i)
		tp.enqueue_work([i]() {
			int x;
			int reps = REPS + (REPS * (rand() % 5));
			for(int n = 0; n < reps; ++n)
			x = i + rand();
		});
	}
	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);
	cout << "simple_thread_pool duration = " << duration.count() / 1000.f << " s" << endl;
	
	srand(0);
	start = high_resolution_clock::now();
	{
		thread_pool tp;
		for(int i = 0; i < COUNT; ++i)
		tp.enqueue_work([i]() {
			int x;
			int reps = REPS + (REPS * (rand() % 5));
			for(int n = 0; n < reps; ++n)
			x = i + rand();
		});
	}
	end = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(end - start);
	cout << "thread_pool duration = " << duration.count() / 1000.f << " s" << endl;
}
