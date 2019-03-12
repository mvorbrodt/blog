#include <iostream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <list>
#include <algorithm>
#include <numeric>
#include <cstdlib>

using namespace std;
using namespace chrono;

const int ELEMS = 10'000'000;

int main(int argc, char** argv)
{
	srand((unsigned int)time(NULL));
	
	using Container1 = list<int>;
	Container1 c;
	c.resize(ELEMS);
	generate(begin(c), end(c), rand);
	
	auto start_time = high_resolution_clock::now();
	accumulate(begin(c), end(c), 0);
	cout  << fixed << setprecision(3);
	cout << "List duration " << duration_cast<microseconds>(high_resolution_clock::now() - start_time).count() / 1000.f << " ms" << endl;
	
	c.sort();
	
	start_time = high_resolution_clock::now();
	accumulate(begin(c), end(c), 0);
	cout << "Sorted list duration " << duration_cast<microseconds>(high_resolution_clock::now() - start_time).count() / 1000.f << " ms" << endl;
	c.clear();
	
	using Container2 = vector<int>;
	Container2 c2;
	c2.resize(ELEMS);
	generate(begin(c2), end(c2), rand);
	
	start_time = high_resolution_clock::now();
	accumulate(begin(c2), end(c2), 0);
	cout << "Vector duration " << duration_cast<microseconds>(high_resolution_clock::now() - start_time).count() / 1000.f << " ms" << endl;
	
	sort(begin(c2), end(c2));
	
	start_time = high_resolution_clock::now();
	accumulate(begin(c2), end(c2), 0);
	cout << "Sorted vector duration " << duration_cast<microseconds>(high_resolution_clock::now() - start_time).count() / 1000.f << " ms" << endl << endl;
}
