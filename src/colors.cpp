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
	//cout << rapid_blink << "RAPID BLINK" << reset << endl;
	cout << inverse << "REVERSE" << reset << endl;
	/*cout << conceal << "CONCEAL" << reset << endl;
	cout << crossed_out << "CROSS OUT" << reset << endl;
	cout << alt_font_1 << "ALT FONT 1" << reset << endl;
	cout << alt_font_2 << "ALT FONT 1" << reset << endl;
	cout << alt_font_3 << "ALT FONT 1" << reset << endl;
	cout << alt_font_4 << "ALT FONT 1" << reset << endl;
	cout << alt_font_5 << "ALT FONT 1" << reset << endl;
	cout << alt_font_6 << "ALT FONT 1" << reset << endl;
	cout << alt_font_7 << "ALT FONT 1" << reset << endl;
	cout << alt_font_8 << "ALT FONT 1" << reset << endl;
	cout << alt_font_9 << "ALT FONT 1" << reset << endl;
	cout << fraktur << "FRAKTUR" << reset << endl;
	cout << doubly_underline << "DOUBLY UNDERLINE" << reset << endl;
	cout << framed << "FRAMED" << reset << endl;
	cout << encircled << "ENCIRCLED" << reset << endl;
	cout << overlined << "OVERLINED" << reset << endl;
	 */
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
