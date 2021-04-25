#include <iostream>
#include <sstream>
#include <iomanip>
#include <locale>
#include <chrono>
#include <random>
#include <atomic>
#include <memory>
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

template<typename PT, bool FAST>
void benchmark(const char* pool_name, uint64_t tasks, uint64_t reps)
{
	cout << black << bold << pool_name << normal << " (" << red << with_commas(tasks) << black << " tasks, " << red << with_commas(reps) << black << " reps)" << flush;

	random_device rd;
	mt19937 mt(rd());

	atomic_uint64_t result = 0;
	auto work = [&](uint64_t r)
	{
		uint64_t sum = 0;
		while(r--)
		{
			auto t = mt();
			sum += t + 1;
			sum -= t;
		}
		result += sum;
	};

	auto start_time = high_resolution_clock::now();
	{
		PT pool;
		for (uint64_t i = 1; i <= tasks; ++i)
		{
			if(FAST) pool.enqueue_work(work, reps);
			else [[maybe_unused]] auto p = pool.enqueue_task(work, reps);
		}
	}
	auto end_time = high_resolution_clock::now();

	cout << "\t" << red << duration_cast<microseconds>(end_time - start_time).count() / 1000.f << " ms" << black;
	cout << "\t(", (result == tasks * reps ? cout << green : cout << red), cout << with_commas(result) << reset << ")" << endl;
}

int main()
{
	uint64_t TASK_START = 100'000;
	uint64_t TASK_STEP  = 100'000;
	uint64_t TASK_STOP  = 1'000'000;

	uint64_t REPS_START = 100;
	uint64_t REPS_STEP  = 100;
	uint64_t REPS_STOP  = 1'000;

	for(auto t = TASK_START; t <= TASK_STOP; t += TASK_STEP)
	{
		if(TASK_START < TASK_STOP)
			cout << "********************************************************************************" << endl;

		for(auto r = REPS_START; r <= REPS_STOP; r += REPS_STEP)
		{
			benchmark<simple_thread_pool, true>("simple   (fast path)", t, r);
			benchmark<thread_pool, true>       ("advanced (fast path)", t, r);
			//cout << endl;
			//benchmark<simple_thread_pool, false>("simple   (slow path)", t, r);
			//benchmark<thread_pool, false>       ("advanced (slow path)", t, r);

			if(REPS_START < REPS_STOP)
				cout << endl;
		}

		if(TASK_START < TASK_STOP)
			cout << endl;
	}
}
