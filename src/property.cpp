#include <iostream>
#include <memory>
#include <vector>
#include "T.hpp"
#include "property.hpp"

using namespace std;

int main()
{
	property<int*> pi = new int[2];
	property<int> pi2;
	cout << pi << endl;
	*pi++ = 1;
	cout << pi << endl;
	*pi++ = 2;
	cout << pi << endl;
	pi -= 1;
	--pi;
	pi += pi2;
	cout << pi << endl;
	cout << pi[0] << ", " << pi[1] << endl << endl;

	property<T> abc;
	cout << abc.invoke(&T::get_instance_number) << endl << endl;
	abc.invoke(&T::foo);

	property<vector<int>> pv1 = make_property<vector<int>>(1, 2, 3, 4, 5);
	pv1[1] = 1;
	pv1.invoke(&vector<int>::size);
	((vector<int>)pv1).size();

	using up_t = std::unique_ptr<T>;
	using sp_t = std::shared_ptr<T>;

	property<up_t> up_p1 = make_unique<T>();
	up_p1->get_instance_number();

	property<sp_t> sp_p2 = make_property<sp_t>(new T);
	sp_p2->get_instance_number();

	return 1;

	auto p1 = make_property<int>(44);
	auto p2 = make_property<int>(66);

	property<const float> p3(p2);
	const property<double> p4(p3), p5(p2);

	p1.add_update_event([](auto p) { cout << "p1 updated with value: " << p << endl; });
	p2.add_update_event([](auto p) { cout << "p2 updated with value: " << p << endl; });
	p3.add_update_event([](auto p) { cout << "p3 updated with value: " << p << endl; });

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
	property<Q> p12(std::move(s));

	p11 = p10;
	p11 = std::move(p10);

	property<Q> p13{"C++20"};
	p12 = p13;
	p12 = std::move(p13);



	property<int*> pp1{new int}, pp2{new int};
	property<const int*> pp3(std::move(pp1));
	*pp2 = 42;
	cout << "property<int*> = " << *pp2 << " -> " << pp2 << endl;
	cout << "property<int*> = " << *pp3 << " -> " << pp3 << endl;
	property<int*> x1;
	x1 = std::move(pp2);

	property<int[]> ap1{new int[3]}, ap2{new int[1]};
	property<const int[]> ap3(std::move(ap1));
	ap2[1] = 42;
	cout << "property<int[]> = " << ap2[1] << " -> " << ap2 << endl;
	cout << "property<int[]> = " << ap3[1] << " -> " << ap3 << endl;
	property<int[]> x2;
	x2 = std::move(ap2);

	property<T*> pp4{new T}, pp5{new T};
	property<const T*> pp6(std::move(pp4));
	cout << "property<T*> = " << *pp5 << " -> " << pp5 << endl;
	cout << "property<T*> = " << *pp6 << " -> " << pp6 << endl;

	property<T[]> ap4{new T[3]}, ap5{new T[1]};
	property<const T[]> ap6(std::move(ap4));
	cout << "property<T[]> = " << ap5[1] << " -> " << ap5 << endl;
	cout << "property<T[]> = " << ap6[1] << " -> " << ap6 << endl;

	property<Q*> pp7{new Q};
	property<T*> pp8(std::move(pp7));
	cout << "property<Q*> = " << *pp8 << " -> " << pp8 << endl;

	property<Q[]> ap7{new Q[1]};
	property<T[]> ap8(std::move(ap7));
	cout << "property<Q[]> = " << *ap8 << " -> " << ap8 << endl;



	property<T> p14({1, 2, 3});
	auto p15 = make_property<T>(4, 5, 6);

	property<Q> p16({7, 8, 9});
	auto p17 = make_property<Q>(10, 11, 12);
}
