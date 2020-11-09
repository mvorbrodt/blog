#include "deep_ptr.hpp"
#include "I.hpp"
#include "T.hpp"

using namespace std;

void f(deep_ptr<T> r) { if(r) r->foo(); }

int main()
{
	struct S
	{
		S() = default;
		S(const S&) = default;
		S(S&&) = default;
	};

	S s1;/*
	deep_ptr<S> xxx{ s1 };
	deep_ptr<S> yyy{ S{} };*/

	using r = deep_ptr<T>;
	using q = deep_ptr<Q>;

	/*r x0{ T{ "by r-value" } };
	T t1{ "by l-value" };
	r x00{ std::move(t1) };*/

	//auto mr1 = make_deep<int>(42);
	//auto mr2 = make_deep<T>("C++ Rocks!");

	r x1{ new T };
	r x2{ new T };
	r x3{ x2 };
	r x4{ std::move(x1) };
	//return;

	q q1;
	q1 = nullptr;
	q1 = q{new Q};

	x1 = q1;

	r rr1{ q1 };
	r rr2{ new Q };

	q1.reset(new Q);
	q1.release();

	q q2;
	r rr3{ std::move(rr2) };

	rr3 = std::move(rr2);

	//return;

	f(nullptr);
	f(r{ new Q{ "C++20" } });

	//return;

	r r1{ new T{ "C++98" } };
	r r2{ new T{ "C++11" } };
	r r3{ new T{ "C++14" } };
	r r4{ new T{ 1, 2, 3} };
	r r5{};
	r r6{ nullptr };

	r1 = r2;
	r2 = std::move(r3);
	r4 = r{ new T{ "C++17" } };
	r5 = r{ new T{ "C++20" } };

	r1->foo();
	r2->bar();

	cout << "\n" << *r4 << "\n\n";
	if(r5) cout << "r5 is not nullptr\n\n";
	if(!r6) cout << "r6 is nullptr\n\n";
}
