#include <iostream>
#include <iomanip>
#include <vector>
#include <latch>
#include <barrier>
#include "token_bucket.hpp"

#define all(c) for(auto& it : c) it

int main()
{
	using namespace std;

	try
	{
		auto N = 1;
		auto bucket = token_bucket(1ms, 1'000);
		auto count = thread::hardware_concurrency();
		auto run = atomic_bool{ true };
		auto total = atomic_uint64_t{};
		auto counts = vector<atomic_uint64_t>(count);
		auto fair_start = latch(count);
		auto fair_play = barrier(count);
		auto threads = vector<thread>(count);

		auto worker = [&](auto x)
		{
			fair_start.arrive_and_wait();
			while (run)
			{
				// fair_play.arrive_and_wait();
				bucket.wait(N);
				/* ++total */ total.fetch_add(N, memory_order_relaxed);
				/* ++counts[x] */ counts[x].fetch_add(N, memory_order_relaxed);
			}
			fair_play.arrive_and_drop();
		};

		for (auto& t : threads)
			t = thread(worker, --count);

		auto stats = thread([&]
		{
			while (run)
			{
				for (auto& count : counts)
					cout << fixed << setprecision(5) << left << "Count:\t"
					<< count.load(memory_order_relaxed) << "\t/\t"
					<< (100.0 * count.load(memory_order_relaxed) / total.load(memory_order_relaxed)) << " %" << endl;
				cout << "Total:\t" << total.load(memory_order_relaxed) << endl << endl;
				this_thread::sleep_for(1s);
			}
		});

		cin.get();
		run = false;
		stats.join();
		all(threads).join();
		//for(auto& t : threads)
		//	t.join();
	}
	catch (exception& ex)
	{
		cerr << ex.what() << endl;
	}
}
