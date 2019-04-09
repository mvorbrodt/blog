#include <iostream>
#include <string>
#include "multi_hash.hpp"

using namespace std;

void arr()
{
	string s1 = "Vorbrodt's C++ Blog";
	string s2 = "Vorbrodt's C++ Blog";
	string s3 = "https://vorbrodt.blog";

	auto h1 = hashN(s1, 3);
	auto h2 = hashN(s2, 3);
	auto h3 = hashN(s3, 3);

	cout << "HashN('" << s1 << "'):" << endl;
	for(auto it : h1) cout << it << endl;
	cout << endl;

	cout << "HashN('" << s2 << "'):" << endl;
	for(auto it : h2) cout << it << endl;
	cout << endl;

	cout << "HashN('" << s3 << "'):" << endl;
	for(auto it : h3) cout << it << endl;
	cout << endl;
}

void temp()
{
	string s1 = "Vorbrodt's C++ Blog";
	string s2 = "Vorbrodt's C++ Blog";
	string s3 = "https://vorbrodt.blog";

	auto [s1h1, s1h2, s1h3] = hashNT<3>(s1);
	auto [s2h1, s2h2, s2h3] = hashNT<3>(s2);
	auto [s3h1, s3h2, s3h3] = hashNT<3>(s3);

	cout << "HashNT('" << s1 << "'):" << endl;
	cout << s1h1 << endl << s1h2 << endl << s1h3 << endl << endl;

	cout << "HashNT('" << s2 << "'):" << endl;
	cout << s2h1 << endl << s2h2 << endl << s2h3 << endl << endl;

	cout << "HashNT('" << s3 << "'):" << endl;
	cout << s3h1 << endl << s3h2 << endl << s3h3 << endl << endl;
}

int main()
{
	arr();
	temp();
}
