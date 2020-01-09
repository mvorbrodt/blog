#include <iostream>
#include <memory>
#include <vector>
#include <map>
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

	p1.add_update_event([](auto p) { cout << "~~~ p1 updated with value: " << p << endl; });

	p1 = p2;
	p1 = p3;
	++p1;
	p1 *= p4;



	// W/ POINTERS
	auto pp1 = make_property<T*>(new T);
	auto pp2 = make_property<Q[]>(new Q[3]);

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



	// W/ COMPLEX TYPES
	property<Q> c1;
	property<T> c2(c1);
	property<T> c3(std::move(c2));

	auto c4 = make_property<T>(1, 2, 3);
	auto c5 = make_property<Q>(4, 5, 6);

	c5.add_update_event([](auto p) { cout << "~~~ c5 updated with value: " << p << endl; });

	c5 = c1;
	c5 = std::move(c1);

	((T&)c5).foo();
	c5.invoke(&T::foo);



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

	((sp&)ps2).use_count();
	ps2.invoke(&sp::use_count);

	ps2->foo();
	ps2->get_instance_number();



	// W/ CONTAINERS
	property<vector<int>> pv1{1, 0, 3};
	auto& pv1r = (vector<int>&)pv1;
	pv1r[1] = 2;
	for_each(begin(pv1r), end(pv1r), [](auto& v) { cout << v << ", "; });
	cout << "(" << pv1.invoke(&vector<int>::size) << ")" << endl;

	property<vector<int>> pv2 = make_property<vector<int>>({4, 0, 6});
	auto& pv2r = (vector<int>&)pv2;
	pv2r[1] = 5;
	for_each(begin(pv2r), end(pv2r), [](auto& v) { cout << v << ", "; });
	cout << "(" << pv2.invoke(&vector<int>::size) << ")" << endl;

	property<map<int, int>> pm;//= make_property<map<int, int>>({make_pair(1, 2)});
	auto& pmr = (map<int, int>&)pm;
	pmr[7] = -1;
	pmr[8] = -2;
	pmr[9] = -3;
	for_each(begin(pmr), end(pmr), [](auto& v) { cout << v.first << " => " << v.second << ", "; });
	cout << "(" << pm.invoke(&map<int, int>::size) << ")" << endl;
}
