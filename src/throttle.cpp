#include <iostream>
#include <iomanip>
#include <vector>
#include <latch>
#include "token_bucket.hpp"

#define all(c) for(auto& it : c) it

int main()
{
	using namespace std;

	try
	{
		auto N = 1;
		auto bucket = token_bucket(1ms, 1'000'000, false);
		auto count = thread::hardware_concurrency();
		auto run = atomic_bool{ true };
		auto total = atomic_uint64_t{};
		auto counts = vector<atomic_uint64_t>(count);
		auto fair_start = latch(count + 1);
		auto threads = vector<thread>(count);

		auto stats = thread([&]
		{
			fair_start.arrive_and_wait();

			auto start = chrono::steady_clock::now();
			auto sec = 0;

			while (run)
			{
				for (auto& count : counts)
					cout << fixed << "Count:\t" << count << "\t/\t" << (100.0 * count / total) << " %" << endl;

				cout << "Total:\t" << total << endl << endl;

				this_thread::sleep_until(start + chrono::seconds(++sec));
			}
		});

		auto worker = [&](auto x)
		{
			fair_start.arrive_and_wait();

			while (run)
			{
				bucket.consume(N);
				total += N;
				counts[x] += N;
			}
		};

		all(threads) = thread(worker, --count);

		cin.get();
		run = false;
		stats.join();
		all(threads).join();
	}
	catch (exception& ex)
	{
		cerr << ex.what() << endl;
	}
}
