#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <vector>
#include <array>
#include <latch>
#include <random>
#include <new>
#include <condition_variable>

#ifdef __cpp_lib_hardware_interference_size
	using std::hardware_constructive_interference_size;
	using std::hardware_destructive_interference_size;
#else
	constexpr std::size_t hardware_constructive_interference_size = 64;
	constexpr std::size_t hardware_destructive_interference_size = 64;
#endif

template<typename T = int, std::size_t N = hardware_constructive_interference_size / sizeof(T)>
struct Numbers
{
	static constexpr auto Size  = sizeof(T);
	static constexpr auto Count = N;
	static constexpr auto Bytes = Size * Count;

	void inc()
	{
		while (lock.test_and_set(std::memory_order_acquire));
		for (auto& n : nums)
			++n;
		lock.clear(std::memory_order_release);
	}

	auto sum()
	{
		T s = {};
		for (auto n : nums)
			s += n;
		return s;
	}

	std::array<T, N> nums = {};
	std::atomic_flag lock = ATOMIC_FLAG_INIT;
};

using Nums = Numbers<>;

#define all(x) for(auto& i : x) i

struct null_mutex
{
	void lock() {}
	void unlock() {}
};
auto nm = null_mutex{};
auto cv = std::condition_variable_any{};

int main()
{
	using namespace std;

	auto str = "Hello C++"s;
	auto proc = [] (string&& str) { cout << str << endl; };
	cout << str << endl;
	proc(std::move(str));
	cout << str << endl;
	return 1;

	thread([&] { cout << "waiting..." << endl; cv.wait(nm); cout << "...done!" << endl; }).detach();
	this_thread::sleep_for(5s);
	cv.notify_one();
	this_thread::sleep_for(1s);
	return 1;

	auto N = default_random_engine{ random_device{}() }() % 1'000'000;
	auto Ts = thread::hardware_concurrency();
	auto fair_start = latch{ Ts };
	auto threads = vector<thread>{ Ts };
	auto nums = Nums{};
	auto expected = Nums::Count * N * Ts;

	cout << Nums::Count << " numbers, " << Nums::Size << " bytes each, " << Nums::Bytes << " total, " << Ts << " threads and " << N << " iterations" << endl;

	all(threads) = thread([&]
	{
		fair_start.arrive_and_wait();
		for (auto i = 0; i < N; ++i)
			nums.inc();
	});

	all(threads).join();

	cout << "expected = " << expected << endl;
	cout << "actual   = " << nums.sum() << endl;
}
