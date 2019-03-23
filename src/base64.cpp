#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include "base64.hpp"
#include "ascii_escape_code.hpp"

using namespace std;
using namespace ascii_escape_code;

using buffer = vector<base64::byte>;

int main(int argc, char** argv)
{
	string input
	{
		"Man is distinguished, not only by his reason, but by this singular passion from other animals, "
		"which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
		"generation of knowledge, exceeds the short vehemence of any carnal pleasure."
	};
	string reference
	{
		"TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
		"IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
		"dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
		"dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
		"ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4="
	};

	try
	{
		vector<base64::byte> data(begin(input), end(input));

		cout << bold << bright_red << "Input: " << reset << italic << input << reset << endl << endl;
		cout << bold << bright_red << "Reference: " << reset << reference << endl << endl;

		auto encoded = base64::encode(data);
		cout << bold << bright_red << "Encoded: " << reset << encoded << endl << endl;

		if(encoded != reference) throw runtime_error("Oh snap! Encoded data does not match reference!");
		else cout << bright_red << "Encoded data matches reference :o)" << reset << endl << endl;

		auto decoded = base64::decode(encoded);
		string s2(begin(decoded), end(decoded));

		cout << bold << bright_red << "Decoded: " << reset << italic << s2 << reset << endl << endl;

		if(data != decoded) throw runtime_error("Oh snap! Input data does not match decoded!");
		else cout << bright_red << "Decoded data matches original :o)" << reset << endl << endl;
	}
	catch(exception& e)
	{
		cerr << slow_blink << bright_red << e.what() << reset << endl << endl;
	}

	return 1;
}
