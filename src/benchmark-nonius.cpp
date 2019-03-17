#include <string>
#define NONIUS_RUNNER
#include <nonius/nonius.h++>
#include <nonius/main.h++>

using namespace std;

string short_string = "hello";
string long_string("0123456789abcdefghijklmnopqrstuvwxyz");

NONIUS_BENCHMARK("StringCopyShort", []
{
	string copy(short_string);
})

NONIUS_BENCHMARK("StringCopyLong", []
{
	string copy(long_string);
})
