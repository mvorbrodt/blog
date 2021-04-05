#include <iostream>
#include <utility>
#include "deep_ptr.hpp"

using namespace std;

struct S
{
	S() = default;
	S(const S&) { cout << "using S(const S&)" << endl; }

	S* clone() const {
		cout << "using S::clone()" << endl;
		return new S; }
};

int main()
{
	auto p1 = make_deep<S>();
	auto p2 = p1;
	auto p3 = std::move(p1);
	p1 = std::move(p3);
	p3 = p2;

	auto clone = [](S* p) { return p->clone(); };
	using clone_ptr = deep_ptr<S, decltype(clone)>;

	auto p4 = clone_ptr(new S, clone);
	auto p5 = p4;
	auto p6 = std::move(p4);
	p4 = std::move(p6);
	p6 = p5;

	auto del = [](S* p) {
		cout << "using 'del' deleter" << endl;
		delete p; };
	using del_ptr = deep_ptr<S, decltype(clone), decltype(del)>;

	auto p7 = del_ptr(new S, clone, del);
	auto p8 = p7;
	auto p9 = std::move(p7);
	p7 = std::move(p9);
	p9 = p8;
}
