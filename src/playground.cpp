#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <iterator>

using namespace std;

int main()
{
	using XnXs = pair<int, int>;
	using Xs = vector<XnXs>;
	using XsPush = back_insert_iterator<Xs>;

	int N = 10, S = 1;
	Xs ints;

	generate_n(XsPush(ints), N, [n = S]() mutable
		{ auto p = make_pair(n, n * n); ++n; return p; });

	for_each(begin(ints), end(ints), [](auto& p)
		{ cout << p.first << ", " << p.second << endl; });
}
