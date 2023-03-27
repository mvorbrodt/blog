#include <iostream>
#include <atomic>
#include <thread>
#include <vector>
#include <array>
#include <latch>
#include <random>

template<typename T = int, std::size_t N = 64 /* std::hardware_constructive_interference_size */ / sizeof(T)>
struct Numbers
{
	static constexpr auto Size  = sizeof(T);
	static constexpr auto Count = N;
	static constexpr auto Bytes = Size * Count;

	void inc()
	{
		while (lock.test_and_set(std::memory_order_relaxed));
		for (auto& n : nums)
			++n;
		lock.clear(std::memory_order_relaxed);
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

int main()
{
	using namespace std;

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
