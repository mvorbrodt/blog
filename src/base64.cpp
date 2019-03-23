#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include "base64.hpp"
#include "ascii_escape_code.hpp"

using namespace std;
using namespace ascii_escape_code;

int main(int argc, char** argv)
{
	string s
	{
		"Man is distinguished, not only by his reason, but by this singular passion from other animals, "
		"which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
		"generation of knowledge, exceeds the short vehemence of any carnal pleasure. ~ Thomas Hobbes' Leviathan"
	};
	vector<base64::byte> data(begin(s), end(s));

	cout << bold << bright_red << "Input: " << reset << italic << s << reset << endl << endl;

	auto encoded = base64::encode(data);
	cout << bold << bright_red << "Encoded: " << reset << encoded << endl << endl;

	auto decoded = base64::decode(encoded);
	string s2(begin(decoded), end(decoded));

	cout << bold << bright_red << "Decoded: " << reset << italic << s2 << reset << endl << endl;

	if(data != decoded) cerr << "Oh snap! Data mismatch!" << endl;
	else cout << "Decoded data matches original :o)" << endl;

	return 1;
}
