#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <algorithm>
#include <pstl/execution>
#include <pstl/algorithm>
using namespace std;
using namespace chrono;
using namespace pstl;

const unsigned long long COUNT = 10'000'000;

int main()
{
	random_device rd;
	mt19937 mt(rd());

	vector<int> data(COUNT);

	auto start = high_resolution_clock::now();

	generate(data.begin(), data.end(), mt);
	sort(data.begin(), data.end());
	is_sorted(data.begin(), data.end());

	auto end = high_resolution_clock::now();
	auto duration = duration_cast<milliseconds>(end - start);
	cout << "STL duration = " << duration.count() / 1000.f << "s" << endl;

	start = high_resolution_clock::now();

	generate(pstl::execution::par_unseq, data.begin(), data.end(), mt);
	sort(pstl::execution::par_unseq, data.begin(), data.end());
	is_sorted(pstl::execution::par_unseq, data.begin(), data.end());

	end = high_resolution_clock::now();
	duration = duration_cast<milliseconds>(end - start);
	cout << "PSTL duration = " << duration.count() / 1000.f << "s" << endl;
}
