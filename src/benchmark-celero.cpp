#include <string>
#include <celero/Celero.h>

using namespace std;

string short_string = "hello";
string long_string("0123456789abcdefghijklmnopqrstuvwxyz");

BASELINE(StringCopyShort, Baseline10, 100, 100'000)
{
	string copy(short_string);
}

BENCHMARK(StringCopyShort, Baseline1000, 100, 1'000'000)
{
	string copy(short_string);
}

BASELINE(StringCopyLong, Baseline10, 100, 100'000)
{
	string copy(long_string);
}

BENCHMARK(StringCopyLong, Baseline1000, 100, 1'000'000)
{
	string copy(long_string);
}

CELERO_MAIN;
