#include <iostream>
#include <iomanip>
#include <vector>
#include <latch>
#include "token_bucket.hpp"

#define all(c) for(auto& it : c) it

int main()
{
	using namespace std;
	using namespace std::chrono;

	try
	{
		auto N = 1;
		auto bucket = token_bucket(1ms, 1'000, true);
		auto count = thread::hardware_concurrency() - 1;
		auto run = atomic_bool{ true };
		auto total = atomic_uint64_t{};
		auto counts = vector<atomic_uint64_t>(count);
		auto fair_start = latch(count + 1);
		auto threads = vector<thread>(count);

		thread([&]
		{
			fair_start.arrive_and_wait();

			auto start = steady_clock::now();
			auto sec = 0;

			while (run)
			{
				auto cnt = 1;
				for (auto& count : counts)
					cout << fixed << "Cnt " << cnt++ << ":\t" << count << "\t / \t" << (100.0 * count / total) << " % \n";

				cout << "Total:\t" << total << "\nTime:\t" << duration_cast<seconds>(steady_clock::now() - start).count() << "s\n" << endl;

				this_thread::sleep_until(start + seconds(++sec));
			}
		}).detach();

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
		all(threads).join();
	}
	catch (exception& ex)
	{
		cerr << ex.what() << endl;
	}
}
