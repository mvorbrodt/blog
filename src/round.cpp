#include <iostream>
#include <iomanip>
#include <array>
#include "round.hpp"

//#define my_round(V, D) runtime_round(V, D)
#define my_round(V, D) constexpr_round<D>(V)

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

	cout << fixed << setprecision(10);

	for(int p = 1; auto v : a)
		cout << p++ << ":\t" << v << endl;
}
