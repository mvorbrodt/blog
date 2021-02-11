#include <iostream>
#include <iomanip>
#include <array>
#include "round.hpp"

using namespace std;

int main()
{
	constexpr auto v = 0.0123456789;
	array<decltype(v), 10> a =
	{
		my_round<1>(v),
		my_round<2>(v),
		my_round<3>(v),
		my_round<4>(v),
		my_round<5>(v),
		my_round<6>(v),
		my_round<7>(v),
		my_round<8>(v),
		my_round<9>(v),
		my_round<10>(v),
	};

	cout << fixed << setprecision(10);

	for(int p = 1; auto it : a)
		cout << p++ << ":\t" << it << endl;
}
