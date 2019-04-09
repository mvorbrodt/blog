#include <iostream>
#include <string>
#include "multi_hash.hpp"

using namespace std;

int main()
{
	string s1 = "Vorbrodt's C++ Blog";
	string s2 = "Vorbrodt's C++ Blog";
	string s3 = "https://vorbrodt.blog";

	auto [s1h1, s1h2, s1h3] = hashN<3>(s1);
	auto [s2h1, s2h2, s2h3] = hashN<3>(s2);
	auto [s3h1, s3h2, s3h3] = hashN<3>(s3);

	cout << "HashN('" << s1 << "'):" << endl;
	cout << s1h1 << ", " << s1h2 << ", " << s1h3 << endl << endl;

	cout << "HashN('" << s2 << "'):" << endl;
	cout << s2h1 << ", " << s2h2 << ", " << s2h3 << endl << endl;

	cout << "HashN('" << s3 << "'):" << endl;
	cout << s3h1 << ", " << s3h2 << ", " << s3h3 << endl << endl;
}
