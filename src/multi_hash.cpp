#include <iostream>
#include <string>
#include "hash.hpp"

using namespace std;

int main()
{
	auto s = "Hash from this string is..."s;

	auto h = hashN(s, 3);

	cout << "hashN(\"" << s << "\", 3);" << endl;
	for(auto it : h) cout << (std::size_t)it << " ";
	cout << endl << endl;

	auto [h1, h2, h3] = hashN<3>(s);

	cout << "hashN<3>(\"" << s << "\");" << endl;
	cout << (std::size_t)h1 << " " << (std::size_t)h2 << " " << (std::size_t)h3 << endl << endl;
}
