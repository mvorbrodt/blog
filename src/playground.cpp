#include <iostream>
#include <iomanip>
#include <array>
#include <cmath>

template<typename T>
T Round(T x, unsigned n)
{
	constexpr static T v[] = {1, 10, 1e2, 1e3, 1e4, 1e5, 1e6, 1e7, 1e8, 1e9, 1e10, 1e11, 1e12};
	return std::round(x * v[n]) / v[n];
}

#define my_round(V, D) Round(V, D)

int main()
{
	using namespace std;

	constexpr auto v = 0.0123456789;
	array<decltype(v), 10> a =
	{
		my_round(v, 1),
		my_round(v, 2),
		my_round(v, 3),
		my_round(v, 4),
		my_round(v, 5),
		my_round(v, 6),
		my_round(v, 7),
		my_round(v, 8),
		my_round(v, 9),
		my_round(v, 10)
	};

	cout << fixed << setprecision(15);

	for(int p = 1; auto v : a)
	cout << p++ << ":\t" << v << endl;
}
