#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <algorithm>
#include <execution>
#include <random>
#include <vector>
#include <cstdint>

TEST_CASE("STL vs PSTL", "[benchmark]")
{
#ifdef _LIBCPP_HAS_PARALLEL_ALGORITHMS
	using namespace std;
	using namespace std::execution;

	auto N = 1'000'000ull;
	auto seed = random_device{}();

	using vec_of_64_bit_ints_t = vector<uint64_t>;

	BENCHMARK("STL")
	{
		auto gen = mt19937_64{ seed };
		auto data = vec_of_64_bit_ints_t(N);
		generate(begin(data), end(data), gen);
		sort(begin(data), end(data));
		is_sorted(begin(data), end(data));
	};

	BENCHMARK("PSTL")
	{
		auto gen = mt19937_64{ seed };
		auto data = vec_of_64_bit_ints_t(N);
		generate(par_unseq, begin(data), end(data), gen);
		sort(par_unseq, begin(data), end(data));
		is_sorted(par_unseq, begin(data), end(data));
	};
#else
	CATCH_ERROR("Parallel STL missing!");
#endif
}
