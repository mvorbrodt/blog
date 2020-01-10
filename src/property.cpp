#include <iostream>
#include <memory>
#include <vector>
#include <array>
#include <map>
#include <string>
#include <algorithm>
#include "T.hpp"
#include "property.hpp"

using namespace std;

int main()
{
	// W/ PRIMITIVE TYPES
	auto p1 = make_property<int>(1);
	auto p2 = make_property<int>(2);

	property<const float> p3(p2);
	const property<double> p4(p3);
	property<int> p5{}, p6{123};

	p1.add_update_event([](auto p) { cout << "~~~ p1 updated with value: " << p << endl; });

	p1 = p2 + 0.f;
	p1 = p3;
	++p1;
	p1 *= p4;



	// W/ POINTERS
	auto pp1 = make_property<T*>(new T);
	auto pp2 = make_property<Q[]>(new Q[3]);
	property<int[]> pp3 = new int[3] {1, 2, 3};

	int* ip1 = new int;
	property<int*> pp4;
	pp4 = ip1;

	*pp1++ = T{"C++11"};
	*pp2++ = Q{"C++14"};

	pp1.add_update_event([](auto p) { cout << "~~~ pp1 updated with value: " << p << " containing: " << *p << endl; });
	pp2.add_update_event([](auto p) { cout << "~~~ pp2 updated with value: " << p << " containing: " << *p << endl; });

	--pp1, pp2 -= 1;

	((T*)pp1)->foo();
	pp1.invoke(&T::foo);
	pp1.invoke(0, &T::bar);

	pp2[0].foo();
	pp2.invoke(1, &Q::foo);
	pp2.invoke(2, &Q::bar);

	pp2[0] = Q{"C++17"};
	pp2[1] = Q{"C++20"};

	delete pp1;
	delete [] pp2;
	delete pp4;



	// W/ COMPLEX TYPES
	property<Q> c1(1, 2, 3);
	property<T> c2(c1);
	property<T> c3(std::move(c2));

	auto c4 = make_property<T>(4, 5, 6);
	auto c5 = make_property<Q>(7, 8, 9);

	property<T> c6{}, c7{"C++Rocks!"}, c8{1, 2, 3};
	property<T> c9{c6}, c10{std::move(c7)};

	c5.add_update_event([](auto p) { cout << "~~~ c5 updated with value: " << p << endl; });

	c5 = c1;
	c5 = std::move(c1);

	((T&)c5).foo();
	c5.invoke(&T::foo);



	// W/ ENUMS
	enum class E : int { E1, E2 = 123, E3 };
	property<E> pe1{E::E2};
	switch(pe1)
	{
		case E::E1: cout << "property<enum E> value is E1" << endl; break;
		case E::E2: cout << "property<enum E> value is E2" << endl; break;
		case E::E3: cout << "property<enum E> value is E3" << endl; break;
	}
	int ec = (int)((E)pe1);
	cout << "property<E> numeric value is " << ec << endl;



	// W/ STRINGS
	property<string> pstr1 = "C++11", pstr2 = " ", pstr3 = "Rocks";
	property<string> pstr4 = pstr1 + pstr2 + pstr3 + " my DUPA!!!";

	pstr1 = "C++17";
	pstr1 += " " + pstr4 + " Rocks!";
	cout << "property<string> value is '" << pstr1 << "'" << endl;



	// W/ SMART POINTERS
	using up = std::unique_ptr<T>;
	using sp = std::shared_ptr<T>;

	up u(new T);
	sp s(new T);

	property<up> pu1(std::move(u));
	property<up> pu2(std::move(pu1));
	property<up> pu3 = std::make_unique<T>();
	property<up> pu4 = make_property<up>(new T);

	property<sp> ps1(s);
	property<sp> ps2(std::move(ps1));
	property<sp> ps3 = std::make_shared<T>();
	property<sp> ps4 = make_property<sp>(new T);

	ps1 = ps2;
	ps2 = std::move(pu2);

	auto use_count = ((sp&)ps2).use_count();
	use_count = ps2.invoke(&sp::use_count);

	ps2->foo();
	ps2->get_instance_number();



	// W/ CONTAINERS
	array<int, 3> arr1 = {1, 0, 3};
	property<array<int, 3>> pa1 = arr1;
	property<array<int, 3>> pa2 = array<int, 3>{1, 0, 3};
	//property<array<int, 3>> pa3 = {1, 2, 3}; // FIX ME~!!!!!111oneone
	pa1[1] = 2;
	auto& pa1r = (array<int, 3>&)pa1;
	cout << "array: ";
	for_each(begin(pa1r), end(pa1r), [](auto& v) { cout << v << ", "; });
	cout << "(" << pa1.invoke(&array<int, 3>::size) << ")" << endl;

	property<vector<int>> pv1{1, 0, 3};
	pv1[1] = 2;
	auto& pv1r = (vector<int>&)pv1;
	cout << "vector: ";
	for_each(begin(pv1r), end(pv1r), [](auto& v) { cout << v << ", "; });
	cout << "(" << pv1.invoke(&vector<int>::size) << ")" << endl;

	property<vector<int>> pv2 = make_property<vector<int>>({4, 0, 6});
	pv2[1] = 5;
	auto& pv2r = (vector<int>&)pv2;
	cout << "vector: ";
	for_each(begin(pv2r), end(pv2r), [](auto& v) { cout << v << ", "; });
	cout << "(" << pv2.invoke(&vector<int>::size) << ")" << endl;

	property<map<int, int>> pm1({{1, 2}, {3, 4}, {5, 6}});
	property<map<int, int>> pm2 = make_property<map<int, int>, pair<const int, int>>({{1, 2}, {3, 4}, {5, 6}});
	pm1 = std::move(pm2);
	pm1[7] = 8;
	pm1[9] = 10;
	pm1[11] = 12;
	auto& pm1r = (map<int, int>&)pm1;
	cout << "map: ";
	for_each(begin(pm1r), end(pm1r), [](auto& v) { cout << v.first << " => " << v.second << ", "; });
	cout << "(" << pm1.invoke(&map<int, int>::size) << ")" << endl;

	property<map<string, int>> pm3({{"_1_", 1}, {"_2_", 2}, {"_3_", 3}});
	pm3["_4_"] = 4;
	pm3["_5_"] = 5;
	auto& pm3r = (map<string, int>&)pm3;
	cout << "map: ";
	for_each(begin(pm3r), end(pm3r), [](auto& v) { cout << v.first << " => " << v.second << ", "; });
	cout << "(" << pm3.invoke(&map<string, int>::size) << ")" << endl;
}
