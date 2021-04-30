#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include "pool.hpp"
#include "ansi_escape_code.hpp"

using namespace std;
using namespace std::chrono;
using namespace ansi_escape_code;

std::string with_commas(uint64_t value)
{
	std::stringstream ss;
	ss.imbue(std::locale(""));
	ss << std::fixed << value;
	return ss.str();
}

template<typename PT>
void benchmark(bool fast_path, const char* pool_name, uint64_t tasks, uint64_t reps, std::size_t pool_threads, std::size_t push_threads)
{
	cout << reset << bold << pool_name << normal << " (" << red << with_commas(tasks) << reset << " tasks, " << red << with_commas(reps) << reset << " reps)" << flush;

	std::atomic_uint check{};
	auto work = [&](uint64_t r)
	{
		volatile uint32_t sum = 0;
		while (r--)
#ifdef _MSC_VER
			++sum;
#else
			asm volatile("inc %0\n\t" : "+r" (sum));
#endif
		check.fetch_add(sum, std::memory_order_relaxed);
	};

	auto start_time = high_resolution_clock::now();
	{
		PT pool{ pool_threads };

		using threads = std::vector<std::thread>;
		threads ts;

		for(int i = 0; i < push_threads; ++i)
		{
			ts.emplace_back([&]
			{
				for (uint64_t i = 1; i <= tasks; ++i)
				{
					if(fast_path) pool.enqueue_work(work, reps);
					else [[maybe_unused]] auto p = pool.enqueue_task(work, reps);
				}
			});
		}

		for(auto& t : ts)
			t.join();
	}
	auto end_time = high_resolution_clock::now();

	auto good = (check == tasks * reps * push_threads);
	cout << "\t" << red << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << " ms" << "\t" << reset;
	if(good) cout << "(" << green << with_commas(check) << reset << ")";
	else cout << "(" << red << with_commas(check) << reset << ")";wcout << blue;
	cout << endl;
}

int main()
{
	auto cores = std::thread::hardware_concurrency();

	uint64_t TASK_START = 0;
	uint64_t TASK_STEP  = 250'000;
	uint64_t TASK_STOP  = 1'000'000;

	uint64_t REPS_START = 0;
	uint64_t REPS_STEP  = 25;
	uint64_t REPS_STOP  = 100;

	for(auto t = TASK_START; t <= TASK_STOP; t += TASK_STEP)
	{
		if(TASK_START < TASK_STOP)
			cout << "********************************************************************************" << endl;

		for(auto r = REPS_START; r <= REPS_STOP; r += REPS_STEP)
		{
			benchmark<simple_thread_pool>(true, "S/fast", !t ? 100'000 : t, !r ? 1 : r, cores, cores / 2);
			benchmark<thread_pool>       (true, "A/fast", !t ? 100'000 : t, !r ? 1 : r, cores, cores / 2);
			// cout << endl;
			// benchmark<simple_thread_pool>(false, "S/slow", !t ? 100'000 : t, !r ? 1 : r, cores, cores / 2);
			// benchmark<thread_pool>       (false, "A/slow", !t ? 100'000 : t, !r ? 1 : r, cores, cores / 2);

			if(REPS_START < REPS_STOP)
				cout << endl;
		}

		if(TASK_START < TASK_STOP)
			cout << endl;
	}
}
