#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <cstdlib>
#include "pool.hpp"
using namespace std;

const int COUNT = 1'000'000;
const int REPS = 10;

TEST_CASE("simple_thread_pool vs thread_pool", "[benchmark]")
{
	BENCHMARK("simple_thread_pool")
	{
		srand(0);
		simple_thread_pool tp;
		for(int i = 0; i < COUNT; ++i)
			tp.enqueue_work([i]()
			{
				int reps = REPS + (REPS * (rand() % 5));
				for(int n = 0; n < reps; ++n)
					int x = i + rand();
			});
	}

	BENCHMARK("thread_pool")
	{
		srand(0);
		thread_pool tp;
		for(int i = 0; i < COUNT; ++i)
			tp.enqueue_work([i]()
			{
				int reps = REPS + (REPS * (rand() % 5));
				for(int n = 0; n < reps; ++n)
					int x = i + rand();
			});
	}
}
