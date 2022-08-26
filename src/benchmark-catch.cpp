#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#if defined(_WIN64)
#include <catch2/catch_all.hpp>
#else
#include <catch2/catch.hpp>
#endif
#include <string>

using namespace std;

string short_string = "hello";
string long_string("0123456789abcdefghijklmnopqrstuvwxyz");

TEST_CASE("string ops", "[benchmark]")
{
	BENCHMARK("StringCopyShort")
	{
		string copy(short_string);
	};

	BENCHMARK("StringCopyLong")
	{
		string copy(long_string);
	};
}