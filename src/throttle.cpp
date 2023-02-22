#include <iostream>
#include <iomanip>
#include <syncstream>
#include <vector>
#include <latch>
#include <barrier>
#include "token_bucket.hpp"

#define ss std::osyncstream(std::cout)

int main()
{
	using namespace std;
	using namespace std::chrono;

	auto count = 3;// thread::hardware_concurrency();
	auto run = atomic_bool{ true };
	auto total = atomic_uint64_t{};
	auto counts = vector<atomic_uint64_t>(count);
	auto threads = vector<jthread>(count);
	auto bucket = token_bucket(3us, 100);

	auto b2 = bucket;
	auto b3 = move(b2);
	bucket = b3;
	bucket = move(b2);
	
	auto fair_start = latch(count);
	auto fair_play = barrier(count);

	auto worker = [&](auto x)
	{
		fair_start.arrive_and_wait();
		while (run)
		{
			//fair_play.arrive_and_wait();
			bucket.consume();
			++total, ++counts[x];
		}
		fair_play.arrive_and_drop();
	};

	for (auto& t : threads)
		t = jthread(worker, --count);

	thread([&]
	{
		while (run)
		{
			this_thread::sleep_for(1.5s);
			for (auto& count : counts)
				ss << fixed << setprecision(5) << left << count << "\t" << (100.0 * count / total) << "%" << endl;
			ss << endl;
		}
	}).detach();

	cin.get();
	run = false;
}
