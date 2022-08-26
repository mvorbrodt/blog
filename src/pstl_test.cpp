#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#if defined(_LIBCPP_HAS_PARALLEL_ALGORITHMS) or defined(_WIN64)
#include <algorithm>
#include <execution>
#include <catch2/catch_all.hpp>
#else
#include <oneapi/dpl/algorithm>
#include <oneapi/dpl/execution>
#include <catch2/catch.hpp>
#endif
#include <random>
#include <vector>
#include <cstdint>

TEST_CASE("STL vs PSTL", "[benchmark]")
{
	using namespace std;
	using namespace std::execution;

	auto gen = mt19937_64{ random_device()() };
	auto data = vector<uint64_t>(1'000'000);

	BENCHMARK("STL")
	{
		generate(begin(data), end(data), gen);
		sort(begin(data), end(data));
		[[maybe_unused]] auto is = is_sorted(begin(data), end(data));
	};

	BENCHMARK("PSTL")
	{
		generate(par_unseq, begin(data), end(data), gen);
		sort(par_unseq, begin(data), end(data));
		[[maybe_unused]] auto is = is_sorted(par_unseq, begin(data), end(data));
	};
}
