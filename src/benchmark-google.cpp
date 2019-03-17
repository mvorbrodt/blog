#include <string>
#include <benchmark/benchmark.h>

using namespace std;

string short_string = "hello";
string long_string("0123456789abcdefghijklmnopqrstuvwxyz");

void BM_StringCopyShort(benchmark::State& state)
{
	for (auto _ : state)
		string copy(short_string);
}
BENCHMARK(BM_StringCopyShort);

void BM_StringCopyLong(benchmark::State& state)
{
	for (auto _ : state)
		string copy(long_string);
}
BENCHMARK(BM_StringCopyLong);

BENCHMARK_MAIN();
