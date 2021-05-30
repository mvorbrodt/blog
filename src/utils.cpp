#include "I.hpp"
#include "T.hpp"
#include "utils.hpp"

using namespace std;
using namespace std::placeholders;

int main()
{
	stack_timer timer;

	auto v = make_vector<T>(5, "C++");

	auto b3 = bind(&T::foo, _1);
	for_each(begin(v), end(v), b3);

	auto b4 = mem_fn(&T::bar);
	for_each(begin(v), end(v), b4);
}
