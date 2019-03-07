#include <iostream>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace chrono;

const int CACHE_LINE_SIZE = 64;//sizeof(int);
const int SIZE = CACHE_LINE_SIZE / sizeof(int) + 1;
const int COUNT = 100'000'000;

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	
	int* p = new int [SIZE];
	
	auto proc = [](int* data) {
		for(int i = 0; i < COUNT; ++i)
			*data = *data + rand();
	};
	
	auto start_time = high_resolution_clock::now();
	
	std::thread t1(proc, &p[0]);
	std::thread t2(proc, &p[SIZE - 1]);
	
	t1.join();
	t2.join();
	
	auto end_time = high_resolution_clock::now();
	cout << "Duration: " << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << " ms" << endl;
	
	return 1;
}
