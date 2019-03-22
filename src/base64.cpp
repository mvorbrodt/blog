#include <iostream>
#include <string>
#include <vector>
#include "base64.hpp"

using namespace std;

int main(int argc, char** argv)
{
	string s{"Vorbrodt's C++ Blog"};
	vector<BYTE> data(begin(s), end(s));

	cout << "Input  : " << s << endl;

	auto encoded = base64::encode(data);
	cout << "Encoded: " << encoded << endl;

	auto decoded = base64::decode(encoded);
	string s2(begin(decoded), end(decoded));

	cout << "Decoded: " << s2 << endl;

	return 1;
}
