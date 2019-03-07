#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <vector>
#include <random>
#include <algorithm>
#include <pstl/execution>
#include <pstl/algorithm>
using namespace std;
using namespace pstl;

const unsigned long long COUNT = 100'000'000;

TEST_CASE("STL vs PSTL", "[benchmark]")
{
	auto seed = random_device{}();

	vector<int> data(COUNT);

	BENCHMARK("STL")
	{
		generate(data.begin(), data.end(), mt19937{seed});
		sort(data.begin(), data.end());
		is_sorted(data.begin(), data.end());
	}

	BENCHMARK("PSTL")
	{
		mt19937 mt(seed);
		generate(pstl::execution::par_unseq, data.begin(), data.end(), mt19937{seed});
		sort(pstl::execution::par_unseq, data.begin(), data.end());
		is_sorted(pstl::execution::par_unseq, data.begin(), data.end());
	}
}
