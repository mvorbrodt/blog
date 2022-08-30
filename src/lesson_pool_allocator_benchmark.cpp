#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include <catch2/catch_all.hpp>

#include <vector>
#include <thread>
#include <string>
#include <random>
#include <algorithm>
#include <boost/pool/singleton_pool.hpp>
#include <cstdint>
#include <cstring>

using namespace std;

using seq_t = uint64_t;
using payload_t = struct { std::byte data[16]; };

static_assert(sizeof(payload_t) >= sizeof(seq_t));

auto gen_next_payload()
{
	static auto seq = seq_t{ 1 };

	payload_t p;
	memcpy(&p, &seq, sizeof(seq));
	++seq;

	return p;
}

struct P
{
	P() : m_v{ gen_next_payload() } {}
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
	NP() : m_v{ gen_next_payload() } {}
	payload_t m_v;
};

static_assert(sizeof(NP) == sizeof(payload_t));

template<typename T>
void run_impl(int c, int n)
{
	static mt19937 gen((random_device{})());

	vector<T*> v;
	v.reserve(n);
	for(auto i = 0; i < c; ++i)
	{
		for(auto it = 0; it < n; ++it)
			v.push_back(new T);

		// simulate randomness in allocations/deallocations
		shuffle(begin(v), end(v), gen);

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
	v.reserve(ts);
	for(auto t = 0; t < ts; ++t)
		v.emplace_back([=]() { run_impl<T>(c, n); });
	for(auto& t : v)
		t.join();
}

TEST_CASE("Pool Allocator. Chunk size = "s + to_string(sizeof(payload_t)), "[benchmark]")
{
	auto C = 10;
	auto N = 100'000;
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
