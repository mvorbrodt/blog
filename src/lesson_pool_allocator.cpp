#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <boost/pool/singleton_pool.hpp>

using namespace std;
using namespace std::chrono;

struct timer
{
	~timer() {
		auto e = high_resolution_clock::now() - m_t;
		cout << duration_cast<milliseconds>(e).count() << " ms" << endl; }

	high_resolution_clock::time_point m_t = high_resolution_clock::now();
};

using payload_t = unsigned long long;
auto payload = payload_t{};

struct P
{
	P() : m_v{ ++payload } {}
	payload_t m_v;

	using pool = boost::singleton_pool<P, sizeof(payload_t)>;

	void* operator new    (std::size_t n) { return pool::malloc(); }
	void* operator new [] (std::size_t n) { return pool::ordered_malloc(n / sizeof(payload_t)); }

	void operator delete    (void* p) { pool::free(p); }
	void operator delete [] (void* p) { pool::free(p); }
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
	timer t;
	run_impl<T>(c, n);
}

template<typename T>
void runN(int c, int n, int ts)
{
	timer t;
	vector<thread> v;
	for(auto t = 0; t < ts; ++t)
		v.emplace_back([=]() { run_impl<T>(c, n); });
	for(auto& t : v)
		t.join();}

int main()
{
	timer runtime;

	auto C = 100'000;
	auto N = 100;
	auto T = thread::hardware_concurrency();

	cout << "run  (" << C << ", " << N << ") w/  pool: ";
	run<P>(C, N);

	cout << "run  (" << C << ", " << N << ") w/o pool: ";
	run<NP>(C, N);

	cout << endl;

	cout << "runN (" << C << ", " << N << ") w/  pool: ";
	runN<P>(C, N, T);

	cout << "runN (" << C << ", " << N << ") w/o pool: ";
	runN<NP>(C, N, T);

	cout << endl << "runtime: ";
}
