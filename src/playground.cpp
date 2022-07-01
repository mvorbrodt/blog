#include <iostream>
#include <string>
#include <tuple>
using namespace std;

auto return_3() { return std::make_tuple("1"s, "2"s, "3"s); }

int main()
{
	const auto& [s1, s2, s3] = return_3();
	cout << s1 << endl << s2 << endl << s3 << endl;
}
