#include <iostream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iterator>
#include <algorithm>
#include <functional>
#include "T.hpp"

using namespace std;
using namespace std::placeholders;

template<typename T, typename... A>
auto make_N_of_T(std::size_t N, A&&... a)
{
	std::vector<T> v;
	v.reserve(N);
	for(std::size_t i = 0; i < N; ++i)
		v.emplace_back(std::forward<A>(a)...);
	return v;
}

int main()
{
	cout << endl;
	auto v = make_N_of_T<T>(5, "C++");

	auto b3 = bind(&T::foo, _1);
	for_each(begin(v), end(v), b3);

	auto b4 = mem_fn(&T::bar);
	for_each(begin(v), end(v), b4);
}
