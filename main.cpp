#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
using namespace std;
using namespace chrono;

const unsigned int COUNT = 1000000000;
const unsigned int THREADS = std::thread::hardware_concurrency();

int main()
{
	auto worker = [](unsigned int count)
	{
		int work = 0;
		for(unsigned int i = 0; i < count; ++i)
			work += i * i;
		return work;
	};

	milliseconds base_line;
	for(int t = 1; t <= THREADS; ++t)
	{
		vector<thread> vt;

		auto start = high_resolution_clock::now();

		for(unsigned int i = 0; i < t; ++i)
			vt.emplace_back(worker, COUNT / t);

		for(auto& t : vt)
			t.join();
		
		auto end = high_resolution_clock::now();
		auto duration = duration_cast<milliseconds>(end - start);
		cout << t << " thread(s) duration = " << duration.count() / 1000.f << " s, speedup factor = ";

		if(t == 1) base_line = duration;

		auto speed_up = (float)base_line.count() / (float)duration.count();
		cout << speed_up << endl;

		if(t == THREADS / 2) cout << "hyper-threading" << endl;
	}
}
