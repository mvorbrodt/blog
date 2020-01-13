#include <iostream>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <functional>
#include <iterator>
#include "T.hpp"

using namespace std;

void foo(const T& t) { cout << "foo(const T&) t = " << t << endl; }
void foo(T&& t) { cout << "foo(T&&) t = " << t << endl; }
template<typename T> void bar(T&& t) { foo(forward<T>(t)); }
//void bar(T&& t) { foo(forward<T>(t)); } // Compile error!

template<typename X>
struct S
{
	using proc_t = function<void(const S&)>;

	void foo(const proc_t&) { cout << "S::foo(const proc_t&)" << endl; }
	void foo(proc_t&&) { cout << "S::foo(proc_t&&)" << endl; }

	void bar(proc_t&& p) { S::foo(forward<proc_t>(p)); }
};

int main()
{
	bar(T{"hi!"});
	cout << endl;
	T ttt{"bye!"};
	bar(ttt);
	cout << ">>> done" << endl << endl;

	S<int> s;
	s.bar([](auto&) {});
	auto p = [](auto&) {};
	s.bar(p);
	cout << ">>> done" << endl << endl;

	vector<T> vt;
	vt.push_back(T{"hi!"});
	vt.resize(2);
	cout << ">>> done" << endl << endl;

	vector<Q> vq;
	vq.push_back(Q{"bye!"});
	vq.resize(2);
	cout << ">>> done" << endl << endl;

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
