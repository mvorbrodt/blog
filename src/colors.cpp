#include <iostream>
#include "ansi_escape_code.hpp"

using namespace std;
using namespace ansi_escape_code;

int main()
{
	cout << bold       << "BOLD"       << reset << endl;
	cout << faint      << "FAINT"      << reset << endl;
	cout << italic     << "ITALIC"     << reset << endl;
	cout << underline  << "UNDERLINE"  << reset << endl;
	cout << slow_blink << "SLOW BLINK" << reset << endl;
	cout << inverse    << "INVERSE"    << reset << endl << endl;

	for(int n = 0; n <= 255; ++n)
		cout << color_n(n) << bold << "X" << reset << color_bg_n(n) << bold << "X" << reset;
	cout << endl << endl;

	for(int r = 0; r <= 255; r+=32)
		for(int g = 0; g <= 255; g+=32)
			for(int b = 0; b <= 255; b+=32)
				cout << color_rgb(r, g, b) << color_bg_rgb(255 - r, 255 - g, 255 - b) << bold << "X";
	cout << reset <<endl << endl;

	cout << "==================================================" << endl << endl;

	wcout << bold       << L"BOLD"       << reset << endl;
	wcout << faint      << L"FAINT"      << reset << endl;
	wcout << italic     << L"ITALIC"     << reset << endl;
	wcout << underline  << L"UNDERLINE"  << reset << endl;
	wcout << slow_blink << L"SLOW BLINK" << reset << endl;
	wcout << inverse    << L"INVERSE"    << reset << endl << endl;

	for(int n = 0; n <= 255; ++n)
		wcout << color_n(n) << bold << L"X" << reset << color_bg_n(n) << bold << L"X" << reset;
	wcout << reset << endl << endl;

	for(int r = 0; r <= 255; r+=32)
		for(int g = 0; g <= 255; g+=32)
			for(int b = 0; b <= 255; b+=32)
				wcout << color_rgb(r, g, b) << color_bg_rgb(255 - r, 255 - g, 255 - b) << bold << L"X";
	wcout << reset <<endl << endl;
}
