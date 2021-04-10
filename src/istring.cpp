#include <iostream>
#include <iomanip>
#include "istring.hpp"
using namespace std;

void f_std(const string& s) {}
void f_is(const istring& s) {}

int main()
{
	string std1{"abc"};
	istring istr1{"ABC"};

	f_std(istr1);
	f_is(std1);

	auto result = bool{};
	result = (std1 == istr1);
	result = (std1 != istr1);
	result = (std1 <  istr1);
	result = (std1 <= istr1);
	result = (std1 > istr1);
	result = (std1 >= istr1);

	auto is1 = istring{ "aaa" };
	auto is2 = istring{ "AAA" };
	cout << "'" << is1 << "' == '" << is2 << "' 2-way: " << boolalpha
		<< (is1 == is2) << ", 3-way: " << is1.compare(is2) << endl;

	is1 = "aaa"_is;
	is2 = "bbb"_is;
	cout << "'" << is1 << "' == '" << is2 << "' 2-way: " << boolalpha
		<< (is1 == is2) << ", 3-way: " << is1.compare(is2) << endl;

	is1 = "bbb"_is;
	is2 = "aaa"_is;
	cout << "'" << is1 << "' == '" << is2 << "' 2-way: " << boolalpha
		<< (is1 == is2) << ", 3-way: " << is1.compare(is2) << endl;

	auto iws1 = iwstring{ L"aaa" };
	auto iws2 = iwstring{ L"AAA" };
	wcout << L"'" << iws1 << L"' == '" << iws2 << L"' 2-way: " << boolalpha
		<< (iws1 == iws2) << L", 3-way: " << iws1.compare(iws2) << endl;

	iws1 = L"aaa"_iws;
	iws2 = L"bbb"_iws;
	wcout << L"'" << iws1 << L"' == '" << iws2 << L"' 2-way: " << boolalpha
		<< (iws1 == iws2) << L", 3-way: " << iws1.compare(iws2) << endl;

	iws1 = L"bbb"_iws;
	iws2 = L"aaa"_iws;
	wcout << L"'" << iws1 << L"' == '" << iws2 << L"' 2-way: " << boolalpha
		<< (iws1 == iws2) << L", 3-way: " << iws1.compare(iws2) << endl;
}
