#include <iostream>
#include <string>
#include "hash.hpp"

using namespace std;

int main()
{
	auto s = "Hash from this string is..."s;

	auto h1 = hashN<std::uint8_t>(s, 3);
	auto h2 = hashN<std::uint16_t>(s, 3);
	auto h3 = hashN(s, 3);

	cout << "hashN<std::uint8_t>(\"" << s << "\", 3);" << endl;
	for(auto it : h1) cout << (std::size_t)it << " ";
	cout << endl << endl;

	cout << "hashN<std::uint16_t>(\"" << s << "\", 3);" << endl;
	for(auto it : h2) cout << (std::size_t)it << " ";
	cout << endl << endl;

	cout << "hashN(\"" << s << "\", 3);" << endl;
	for(auto it : h3) cout << (std::size_t)it << " ";
	cout << endl << endl;

	auto [s1h1, s1h2, s1h3] = hashNT<3, std::uint8_t>(s);
	auto [s2h1, s2h2, s2h3] = hashNT<3, std::uint16_t>(s);
	auto [s3h1, s3h2, s3h3] = hashNT<3>(s);

	cout << "hashNT<3, std::uint8_t>(\"" << s << "\", 3);" << endl;
	cout << (std::size_t)s1h1 << " " << (std::size_t)s1h2 << " " << (std::size_t)s1h3 << endl << endl;

	cout << "hashNT<3, std::uint16_t>(\"" << s << "\", 3);" << endl;
	cout << (std::size_t)s2h1 << " " << (std::size_t)s2h2 << " " << (std::size_t)s2h3 << endl << endl;

	cout << "hashNT<3>(\"" << s << "\", 3);" << endl;
	cout << (std::size_t)s3h1 << " " << (std::size_t)s3h2 << " " << (std::size_t)s3h3 << endl << endl;
}
