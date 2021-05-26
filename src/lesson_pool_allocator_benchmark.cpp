#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch.hpp>

#include <vector>
#include <thread>
#include <string>
#include <boost/pool/singleton_pool.hpp>

using namespace std;

using payload_t = unsigned long long;
auto payload = payload_t{};

struct P
{
	P() : m_v{ ++payload } {}
	payload_t m_v;

	using pool = boost::singleton_pool<P, sizeof(payload_t)>;

	void* operator new    (std::size_t n) { return pool::malloc(); }
	void* operator new [] (std::size_t n) = delete;

	void operator delete    (void* p) { pool::free(p); }
	void operator delete [] (void* p) = delete;
};

static_assert(sizeof(P) == sizeof(payload_t));

struct NP
{
	NP() : m_v{ ++payload } {}
	payload_t m_v;
};

static_assert(sizeof(NP) == sizeof(payload_t));

template<typename T>
void run_impl(int c, int n)
{
	vector<T*> v;
	v.reserve(n);
	for(auto i = 0; i < c; ++i)
	{
		for(auto it = 0; it < n; ++it)
			v.push_back(new T);
		for(auto p : v)
			delete p;
		v.clear();
	}
}

template<typename T>
void run(int c, int n)
{
	run_impl<T>(c, n);
}

template<typename T>
void runN(int c, int n, int ts)
{
	vector<thread> v;
	for(auto t = 0; t < ts; ++t)
		v.emplace_back([=]() { run_impl<T>(c, n); });
	for(auto& t : v)
		t.join();
}

TEST_CASE("Pool Allocator", "[benchmark]")
{
	auto C = 1'000;
	auto N = 100;
	auto T = thread::hardware_concurrency();

	BENCHMARK("1 thread w/ pool")
	{
		run<P>(C, N);
	};
	BENCHMARK("1 thread w/o pool")
	{
		run<NP>(C, N);
	};
	BENCHMARK(to_string(T) + " threads, w/ pool")
	{
		runN<P>(C, N, T);
	};
	BENCHMARK(to_string(T) + " threads,  w/o pool")
	{
		runN<NP>(C, N, T);
	};
}
