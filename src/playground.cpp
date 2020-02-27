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

template<typename U>
decltype(auto) foo(U&& v)
{
	return forward<U>(v);
}

int main()
{
	T t0{"C++"};
	decltype(auto) t1 = (t0);
	decltype(auto) t2 = foo(t1);
}
