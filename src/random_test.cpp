#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <random>

using namespace std;

const int COUNT = 10'000'000;

TEST_CASE("random", "[benchmark]")
{
	random_device rd;
	mt19937 mt(rd());
	uniform_int_distribution int_dist(-10, 10);
	uniform_real_distribution real_dist(1.0, 10.0);

	int result{0};

	BENCHMARK("random_device")
	{
		for(int i = 0; i < COUNT; ++i)
			result += rd();
	}

	BENCHMARK("mt19937")
	{
		for(int i = 0; i < COUNT; ++i)
			result += mt();
	}

	BENCHMARK("uniform_int_distribution")
	{
		for(int i = 0; i < COUNT; ++i)
			result += int_dist(mt);
	}

	BENCHMARK("uniform_real_distribution")
	{
		for(int i = 0; i < COUNT; ++i)
			result += real_dist(mt);
	}
}
