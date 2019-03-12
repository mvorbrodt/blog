#include <iostream>
#include <mutex>
#include <vector>
#include <algorithm>
#include <cstdlib>
using namespace std;

mutex cout_lock;
#define trace(x) { scoped_lock<mutex> lock(cout_lock); cout << x << endl; }

const int COUNT = 10;

struct range
{
	unsigned int lo;
	unsigned int hi;
	
	bool in_range(unsigned int p) const { return lo <= p && p < hi; }
};

bool operator < (const range& lhs, const range& rhs)
{
	return lhs.lo < rhs.lo;
}

ostream& operator << (ostream& os, const range& r)
{
	os << "LO: " << r.lo << ", HI: " << r.hi;
	return os;
}

range BinarySearch(const vector<range>& v, unsigned int p)
{
	size_t index = v.size() / 2;
	size_t step = index / 2 + 1;
	
	while(true)
	{
		if(v[index].hi <= p) index += step;
		if(v[index].lo > p) index -= step;
		step /= 2;
		if(step == 0) step = 1;
		if(v[index].in_range(p)) break;
	}
	
	return v[index];
}

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	
	vector<range> ranges =
	{
		{50, 60}, {60, 70}, {70, 80}, {80, 90}, {90, 100},
		{0, 10}, {10, 20}, {20, 30}, {30, 40}, {40, 50}
	};
	
	sort(begin(ranges), end(ranges));
	
	for(const auto& r : ranges)
		trace(r);
	
	for(int i = 0; i < COUNT; ++i)
	{
		unsigned int p = rand() % 100;
		trace("P = " << p << " falls in range " << BinarySearch(ranges, p));
	}
	
	return 1;
}
