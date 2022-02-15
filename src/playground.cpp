#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <latch>
#include <barrier>
#include <unordered_set>
using namespace std;

struct obj
{
	int ID = 0;
	string STR = ""s;
};

using obj_set = unordered_set
<
	obj,
	decltype([](const auto& it) { return hash<size_t>()(it.ID); }), // hash
	decltype([](const auto& lhs, const auto& rhs) { return lhs.ID == rhs.ID; }) // equality
>;

auto l = std::latch(3);
auto b = std::barrier(3);

int main()
{
	obj_set s;

	s.insert({1, "STR 1"});
	s.insert({2, "STR 2"});
	s.insert({3, "STR 3"});

	cout << s.find({1})->STR << endl;
	cout << s.find({2})->STR << endl;
	cout << s.find({3})->STR << endl;
}
