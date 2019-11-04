#include <iostream>
#include <vector>
#include <range/v3/range.hpp>
#include <range/v3/view.hpp>

using namespace std;
namespace r = ranges;
namespace rv = ranges::views;

int main(int argc, char** argv)
{
	using Ints = vector<int>;
	Ints numbers = { 1, 2, 3, 4, 5 };

	auto evenNumbers = numbers
		| rv::filter([](int n){ return n % 2 == 0; })
		| rv::transform([](int n) { return n * 2; });

	for(auto _ : evenNumbers)
		cout << _ << " ";
	cout << endl;

	auto xs = rv::ints(1, 11) // Make int range, 1 to 10... 11 because stl ranges exclude last element
		| rv::transform([](auto i) { return std::make_pair(i, i * i); }) // transform each int in range
		| r::to<vector>(); // convert generated range to standard vector

	for_each(begin(xs), end(xs), [](auto& p) { cout << p.first << ", " << p.second << endl; });

	return 1;
}
