#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
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
		generate(begin(data), end(data), mt19937{ seed });
		sort(begin(data), end(data));
		is_sorted(begin(data), end(data));
	};

	BENCHMARK("PSTL")
	{
		generate(pstl::execution::par_unseq, begin(data), end(data), mt19937{ seed });
		sort(pstl::execution::par_unseq, begin(data), end(data));
		is_sorted(pstl::execution::par_unseq, begin(data), end(data));
	};
}
