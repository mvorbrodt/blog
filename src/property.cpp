#include <iostream>
#include "property.hpp"
#include "T.hpp"

using namespace std;

int main()
{
	property<int> p1 = 44, p2 = 66;
	property<float> p3(p2);
	const property<double> p4(p3), p5(p2);

	p1 += [](auto new_value) { cout << "p1 updated with value: " << new_value << endl; };
	p2 += [](auto new_value) { cout << "p2 updated with value: " << new_value << endl; };
	p3 += [](auto new_value) { cout << "p3 updated with value: " << new_value << endl; };

	p2 = p1;
	p2 = move(p1);
	p2 = p3;
	p2 = move(p3);

	int x = 123;
	p1 = 123;
	p1 = 123.f;
	p1 = x;

	const property<int> cp1(12345);
	property<int> p6(cp1);
	property<float> p7(cp1);
	property<float> p8(std::move(p1));
	property<float> p9(std::move(p8));
	int xyz = cp1;

	cout << cp1 << ", " << p2 << ", " << p3 << endl;

	property<T> p10{"C++11"};
	string s = "C++17";
	property<T> p11(s);

	p11 = p10;
	p11 = std::move(p10);
}
