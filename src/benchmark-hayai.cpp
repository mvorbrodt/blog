#include <string>
#include <hayai/hayai.hpp>

using namespace std;

string short_string = "hello";
string long_string("0123456789abcdefghijklmnopqrstuvwxyz");

BENCHMARK(StringOps, StringCopyShort, 100, 1'000'000)
{
	string copy(short_string);
}

BENCHMARK(StringOps, StringCopyLong, 100, 100'000)
{
	string copy(long_string);
}
