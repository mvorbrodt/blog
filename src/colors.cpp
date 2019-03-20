#include <iostream>
#include "ascii_escape_codes.hpp"

using namespace std;
using namespace ascii_escape_codes;

int main(int argc, char** argv)
{
	cout << reset;
	cout << bold << "BOLD" << reset << endl;
	cout << faint << "FAINT" << reset << endl;
	cout << italic << "ITALIC" << reset << endl;
	cout << underline << "UNDERLINE" << reset << endl;
	cout << slow_blink << "SLOW BLINK" << reset << endl;
	cout << inverse << "REVERSE" << reset << endl;
	cout << endl;

	for(int n = 0; n <= 255; ++n)
		cout << color_n(n) << "X";
	cout << reset << endl << endl;

	for(int n = 0; n <= 255; ++n)
		cout << color_bg_n(n) << "X";
	cout << reset << endl << endl;

	for(int r = 0; r <= 255; r+=32)
		for(int g = 0; g <= 255; g+=32)
			for(int b = 0; b <= 255; b+=32)
				cout << color_rgb(r, g, b) << "X";
	cout << reset <<endl << endl;

	for(int r = 0; r <= 255; r+=32)
		for(int g = 0; g <= 255; g+=32)
			for(int b = 0; b <= 255; b+=32)
				cout << color_bg_rgb(r, g, b) << "X";
	cout << reset <<endl << endl;

	return 1;
}
