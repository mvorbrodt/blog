#include <iostream>

template<typename Proc>
class out_of_line
{
public:
	explicit out_of_line(Proc proc) : m_proc(proc) { std::cout << "out_of_line\n"; }
	~out_of_line() { std::cout << "~out_of_line\n"; }

	template<typename Ctx>
	auto operator()(const Ctx& ctx) const
	{
		return [&ctx, this] (auto&&... args) { return m_proc(ctx, std::forward<decltype(args)>(args)...); };
	}

	template<typename Ctx>
	auto operator()(Ctx&& ctx) const
	{
		return [ctx = std::move(ctx), this] (auto&&... args) { return m_proc(ctx, std::forward<decltype(args)>(args)...); };
	}

private:
	Proc m_proc;
};

void api(void*) { std::cout << "api(void*)\n"; }
void api(std::nullptr_t) { std::cout << "api(std::nullptr_t)\n"; }
void api(auto*) = delete; // C++20
// template<typename ...T> void api(T&&...) = delete; // prior to C++20

struct E1 {};
struct E2 {};
template<typename T1, typename T2>
struct E
{
	[[no_unique_address]] T1 m_e1;
	[[no_unique_address]] T2 m_e2;
};



int main()
{
	using namespace std;

	E<E1, E2> e1;
	cout << sizeof(e1) << endl;

	auto&& ool = out_of_line([] (int x) { cout << "proc x = " << x << "\n"; });
	ool(1)();

	void* p = nullptr;
	api(nullptr);
	api(p);
	//int x = {};
	//api(&x);
}
