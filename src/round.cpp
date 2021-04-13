#include <iostream>
#include <iomanip>
#include <array>
#include "round.hpp"

//#define my_round(V, D) runtime_round1(V, D)
//#define my_round(V, D) runtime_round2(V, D)
#define my_round(V, D) constexpr_round<D>(V)

int main()
{
	using namespace std;

	constexpr auto v = 0.555555555555555;

	using T = decltype(v);

	auto a = array<T, 15>
	{
		 my_round(v, 1),  my_round(v, 2),  my_round(v, 3),  my_round(v, 4),  my_round(v, 5),
		 my_round(v, 6),  my_round(v, 7),  my_round(v, 8),  my_round(v, 9), my_round(v, 10),
		my_round(v, 11), my_round(v, 12), my_round(v, 13), my_round(v, 14), my_round(v, 15),
	};

	for(auto p = 1; auto v : a)
	{
		cout << fixed << setprecision(p + 1);
		cout << p++ << ":\t" << v << endl;
	}
}
