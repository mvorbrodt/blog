#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#if defined(_LIBCPP_HAS_PARALLEL_ALGORITHMS) or (defined(_WIN64) and defined(_HAS_CXX17))
#include <algorithm>
#include <execution>
#else
#include <pstl/algorithm>
#include <pstl/execution>
#endif
#include <random>
#include <vector>
#include <cstdint>

TEST_CASE("STL vs PSTL", "[benchmark]")
{
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
		[[maybe_unused]] auto is = is_sorted(begin(data), end(data));
	};

	BENCHMARK("PSTL")
	{
		auto gen = mt19937_64{ seed };
		auto data = vec_of_64_bit_ints_t(N);
		generate(par_unseq, begin(data), end(data), gen);
		sort(par_unseq, begin(data), end(data));
		[[maybe_unused]] auto is = is_sorted(par_unseq, begin(data), end(data));
	};
}
