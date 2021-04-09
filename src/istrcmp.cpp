#include <iostream>
#include "istrcmp.hpp"

void f_std(const std::string& s) { std::cout << s << std::endl; }

void f_is(const istring& s) { std::cout << s << std::endl; }

int main()
{
	using namespace std;

	string std1{"abc"};
	istring istr1{"ABC"};
	auto result = (std1 == istr1);

	f_std(istr1);
	f_is(std1);

	auto s1 = "abc"s;
	auto s2 = "ABC"s;
	cout << "'" << s1 << "' == '" << s2 << "' str_icmp: " << str_icmp(s1, s2) << endl;

	s1 = "aaa"s;
	s2 = "bbb"s;
	cout << "'" << s1 << "' == '" << s2 << "' str_icmp: " << str_icmp(s1, s2) << endl;

	s1 = "bbb"s;
	s2 = "aaa"s;
	cout << "'" << s1 << "' == '" << s2 << "' str_icmp: " << str_icmp(s1, s2) << endl << endl;

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
		<< (is1 == is2) << ", 3-way: " << is1.compare(is2) << endl << endl;

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
