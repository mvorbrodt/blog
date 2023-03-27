#include <algorithm>
#include <execution>
#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cstdint>

int main()
{
	using namespace std;
	using namespace std::chrono;
	using namespace std::execution;

	auto gen = mt19937_64{ random_device()() };
	auto data = vector<uint64_t>(250'000'000);

	auto start = steady_clock::now();

	generate(begin(data), end(data), gen);
	sort(begin(data), end(data));
	is_sorted(begin(data), end(data));

	auto stop = steady_clock::now();
	cout << "STL  took " << duration_cast<seconds>(stop - start) << endl;

	start = steady_clock::now();

	generate(par_unseq, begin(data), end(data), gen);
	sort(par_unseq, begin(data), end(data));
	is_sorted(par_unseq, begin(data), end(data));

	stop = steady_clock::now();
	cout << "PSTL took " << duration_cast<seconds>(stop - start) << endl;
}
