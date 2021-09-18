#include <iostream>
//#include <syncstream>
#include <semaphore>
#include <experimental/coroutine>
#include <future>
#include <thread>
#include <barrier>
#include <latch>
#include <memory>

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
template<typename ...T> void api(T&&...) = delete; // prior to C++20

struct E1 {};
struct E2 {};
template<typename T1, typename T2>
struct E
{
	[[no_unique_address]] T1 m_e1;
	[[no_unique_address]] T2 m_e2;
};

template<typename T, typename PD>
requires(not std::is_pointer_v<T> and not std::is_array_v<T>)
struct P
{
	P(T v, PD p = PD()) : m_v{v}, m_p{p} {}
	~P() { m_p(m_v); }

private:
	T m_v;
	[[no_unique_address]] PD m_p;
};

P<int, decltype([](int v) { std::cout << "deleting " << v << std::endl; })> p1{20};



/*auto gen(int v = 0)
{
	while(true)
		co_yield v++;
}*/

#include <iostream>
#include <memory>
#include <type_traits>

using HANDLE = std::byte*;

HANDLE make_HANDLE() { static auto k_h = HANDLE{}; return ++k_h; } // AKA Win32's CreateFile etc
void close_HANDLE(HANDLE h) { std::cout << "HANDLE_close(" << h << ")" << std::endl; } // AKA Win32's CloseHandle etc

#if __cplusplus >= 202002 // C++20; using lambda in unevaluated context
using HANDLE_ptr = std::unique_ptr<
	std::remove_pointer_t<HANDLE>,
	decltype([](HANDLE h) { close_HANDLE(h); })>;
#else
[[maybe_unused]] inline static auto HANDLE_deleter = [](HANDLE h) { close_HANDLE(h); };
using HANDLE_ptr = std::unique_ptr<std::remove_pointer_t<HANDLE>, decltype(HANDLE_deleter)>;
#endif

int main()
{
	auto h1 = HANDLE_ptr(make_HANDLE());
	auto h2 = HANDLE_ptr(make_HANDLE());
	auto h3 = HANDLE_ptr(make_HANDLE());
	std::cout << h1 << std::endl;
	std::cout << h2 << std::endl;
	std::cout << h3 << std::endl;
}

/*
int main()
{
	using namespace std;
	std::counting_semaphore cs = { 5 };
	std::binary_semaphore bs = { 1 };
	osyncstream(cout) cout << cs.max() << endl << bs.max() << endl << endl;

	E<E1, E2> e1;
	cout << "sizeof(E) with 2 [[no_unique_address]] = " << sizeof(e1) << endl;

	auto&& ool = out_of_line([] (int x) { cout << "proc x = " << x << "\n"; });
	cout << "sizeof(out_of_line) instance = " << sizeof (ool) << endl;
	ool(1)();

	void* p = nullptr;
	api(nullptr);
	api(p);
	//int x = {};
	//api(&x);

	std::barrier b{1, [] { cout << "barrier hit!" << endl; }};
	b.arrive_and_wait();

	std::latch l{ 1 };
	l.arrive_and_wait();
	cout << "latch hit!" << endl;
}
*/
