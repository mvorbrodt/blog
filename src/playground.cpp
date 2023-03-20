#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <utility>

class leaky_bucket
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;

	using atomic_duration = std::atomic<duration>;
	using atomic_time_point = std::atomic<time_point>;

	using leak_proc = std::function<void(leaky_bucket&)>;

	template<typename Proc>
	leaky_bucket(std::size_t leaks_per_second, std::size_t capacity, Proc&& proc)
	: leaky_bucket(std::chrono::duration_cast<duration>(std::chrono::seconds(1)) / leaks_per_second, capacity, std::forward<Proc>(proc)) {}

	template<typename Proc>
	leaky_bucket(duration time_per_leak, std::size_t capacity, Proc&& proc)
	: m_time_per_leak{ time_per_leak }, m_capacity{ capacity }, m_proc{ std::forward<Proc>(proc) }
	{
		if (time_per_leak.count() <= 0) throw std::invalid_argument("Invalid leak rate!");
		if (!capacity) throw std::invalid_argument("Invalid bucket capacity!");

		m_leak_thread = std::thread([this]
		{
			std::unique_lock guard(m_bucket_lock);

			auto next_tick_time = clock::now();

			while (true)
			{
				auto leak = m_signal.wait_until(guard, next_tick_time += rate(),
					[&] { return (clock::now() >= next_tick_time && count() > 0) || m_exit.test(); });

				if (m_exit.test())
					break;

				if (leak)
				{
					--m_count;
					m_proc(*this);
				}
			}
		});
	}

	~leaky_bucket()
	{
		m_exit.test_and_set();
		m_signal.notify_one();
		m_leak_thread.join();
	}

	duration rate() const noexcept { return m_time_per_leak; }

	std::size_t capacity() const noexcept { return m_capacity; }

	std::size_t count() const noexcept { return m_count; }

	[[nodiscard]] bool try_pour(std::size_t drops = 1, std::size_t* overflow = nullptr) noexcept
	{
		std::unique_lock guard(m_bucket_lock);

		if (count() + drops > capacity())
		{
			if (overflow != nullptr)
				*overflow = count() + drops - capacity();

			return false;
		}

		m_count += drops;

		return true;
	}

	void pour(std::size_t drops = 1) noexcept
	{
		while(!try_pour(drops))
			std::this_thread::yield();
	}

	void wait(std::size_t drops = 1) noexcept
	{
		std::size_t overflow;
		while (!try_pour(drops, &overflow))
			std::this_thread::sleep_for(rate() * overflow);
	}

private:
	duration m_time_per_leak;

	std::size_t m_capacity;
	std::size_t m_count = 0;

	leak_proc m_proc;

	std::thread m_leak_thread;
	std::recursive_mutex m_bucket_lock;
	std::condition_variable_any m_signal;

	std::atomic_flag m_exit = ATOMIC_FLAG_INIT;
};



#include <iostream>

int main()
{
	using namespace std;
	using namespace std::chrono_literals;

	leaky_bucket bucket(1s, 5, [](leaky_bucket& lb)
	{
		cout << "leaked! drops remaining = " << lb.count() << endl;
	});

	for(auto i = 1; i <= 10; ++i)
	{
		bucket.pour();
		cout << "poured " << i << endl;
	}

	while (bucket.count())
		std::this_thread::yield();

	cout << "waiting..." << endl;
	std::this_thread::sleep_for(3s);

	cout << "pouring " << bucket.capacity() << endl;
	bucket.pour(bucket.capacity());

	while (bucket.count()) std::this_thread::yield();
	cout << "done" << endl;
}
