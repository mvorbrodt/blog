#include <iostream>
#include <vector>
#include <range/v3/view.hpp>

using namespace std;

int main(int argc, char** argv)
{
	std::vector<int> numbers = { 1, 2, 3 ,4, 5 };

	auto evenNumbers = numbers
		| ranges::view::filter([](int n){ return n % 2 == 0; })
		| ranges::view::transform([](int n) { return n * 2; });

	for(auto _ : evenNumbers)
		cout << _ << " ";
	cout << endl;

	return 1;
}
