#include <iostream>
#include <memory>
#include <vector>
#include "T.hpp"
#include "property.hpp"

using namespace std;

int main()
{
	// W/ PRIMITIVE TYPES
	auto p1 = make_property<int>(1);
	auto p2 = make_property<int>(2);

	property<const float> p3(p2);
	const property<double> p4{p3};

	p1.add_update_event([](auto p) { cout << "~~~ p1 updated with value: " << p << endl; });

	p1 = p2;
	p1 = p3;
	++p1;
	p1 *= p4;

	// W/ COMPLEX TYPES
	property<Q> t1;
	property<T> t2(std::move(t1));

	Q q;
	T t(std::move(q));
	t = std::move(q);
	//property<T> t3(std::move(t1));
	//t2 = t3;
	//t3 = std::move(t1);
	/*auto p5 = make_property<T>(1, 2, 3);
	auto p6 = make_property<Q>(4, 5, 6);

	property<T> p7{p5};
	property<Q> p8{std::move(p6)};

	p5.add_update_event([](auto p) { cout << "~~~ p5 updated with value: " << p << endl; });

	p5 = p7;
	p5 = std::move(p8);

	((T&)p5).foo();
	p5.invoke(&T::foo);*/

/*
	property<int*> pp1{new int}, pp2{new int};
	property<const int*> pp3(std::move(pp1));
	*pp2 = 42;
	cout << "property<int*> = " << *pp2 << " -> " << pp2 << endl;
	cout << "property<int*> = " << *pp3 << " -> " << pp3 << endl;
	property<int*> x1;
	x1 = std::move(pp2);

	property<T*> pp4{new T}, pp5{new T};
	property<const T*> pp6(std::move(pp4));
	cout << "property<T*> = " << *pp5 << " -> " << pp5 << endl;
	cout << "property<T*> = " << *pp6 << " -> " << pp6 << endl;

	property<Q*> pp7{new Q};
	property<T*> pp8(std::move(pp7));
	cout << "property<Q*> = " << *pp8 << " -> " << pp8 << endl;
*/
}
