#include <atomic>
#include <chrono>
#include <thread>
#include <utility>
#include <stdexcept>

// Single-Threaded Version of Leaky Bucket as a Counter
class leaky_bucket_st
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;

	leaky_bucket_st(std::size_t drops_per_second, std::size_t drop_capacity, bool full = false)
	: leaky_bucket_st(std::chrono::duration_cast<duration>(std::chrono::seconds(1)) / drops_per_second, drop_capacity, full) {}

	leaky_bucket_st(duration time_per_drop, std::size_t drop_capacity, bool full = false)
	: m_time_per_drop{ time_per_drop }, m_drop_capacity{ drop_capacity }, m_drop_count{ full ? drop_capacity : 0 }
	{
		if (time_per_drop.count() <= 0)
			throw std::invalid_argument("Invalid drop rate!");

		if (!drop_capacity)
			throw std::invalid_argument("Invalid drop capacity!");
	}

	duration rate() const noexcept { return m_time_per_drop; }

	std::size_t capacity() const noexcept { return m_drop_capacity; }

	std::size_t count() const noexcept { return m_drop_count; }

	bool empty() const noexcept { return count() == 0; }

	bool full() const noexcept { return count() == capacity(); }

	void set_rate(std::size_t drops_per_second)
	{
		set_rate(std::chrono::duration_cast<duration>(std::chrono::seconds(1)) / drops_per_second);
	}

	void set_rate(duration time_per_drop)
	{
		if (time_per_drop.count() <= 0)
			throw std::invalid_argument("Invalid drop rate!");

		m_time_per_drop = time_per_drop;
	}

	void set_capacity(std::size_t drop_capacity)
	{
		if (!drop_capacity)
			throw std::invalid_argument("Invalid drop capacity!");

		m_drop_capacity = drop_capacity;
	}

	void drain() noexcept { m_drop_count = 0; }

	void refill() noexcept { m_drop_count = capacity(); }

	bool try_pour(std::size_t drops = 1, duration* time_needed = nullptr)
	{
		if (drops > capacity())
			throw std::logic_error("Invalid drop count!");

		auto new_count = count() + drops;

		if (new_count > capacity())
		{
			if (time_needed != nullptr)
				*time_needed = rate() * (new_count - capacity());

			return false;
		}

		m_drop_count = new_count;

		return true;
	}

	void pour(std::size_t drops = 1)
	{
		while (!try_pour(drops))
			std::this_thread::yield();
	}

	void pour_or_wait(std::size_t tokens = 1)
	{
		duration time_needed;
		while (!try_pour(tokens, &time_needed))
			std::this_thread::sleep_for(time_needed);
	}

	[[nodiscard]] bool try_leak(std::size_t drops = 1, duration* time_needed = nullptr)
	{
		if (drops > capacity())
			throw std::logic_error("Invalid drop count!");

		auto now = clock::now();
		auto delay = drops * rate();
		auto new_time = now + delay;

		if (empty() || drops > count())
		{
			if (time_needed != nullptr)
				*time_needed = rate();

			return false;
		}

		if (now < m_time)
		{
			if (time_needed != nullptr)
				*time_needed = m_time - now;

			return false;
		}

		m_time = new_time;
		m_drop_count -= drops;

		return true;
	}

	void leak(std::size_t drops = 1)
	{
		while (!try_leak(drops))
			std::this_thread::yield();
	}

	void leak_or_wait(std::size_t tokens = 1)
	{
		duration time_needed;
		while (!try_leak(tokens, &time_needed))
			std::this_thread::sleep_for(time_needed);
	}

private:
	time_point m_time = {};
	duration m_time_per_drop;

	std::size_t m_drop_capacity;
	std::size_t m_drop_count;
};





// Multi-Threaded Version of Leaky Bucket as a Counter
class leaky_bucket_mt
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;

	using atomic_duration = std::atomic<duration>;
	using atomic_time_point = std::atomic<time_point>;

	using state = std::pair<duration, size_t>;
	using atomic_state = std::atomic<state>;

private:
	atomic_time_point m_time = {};
	atomic_duration m_time_per_drop;

	std::atomic_size_t m_drop_capacity;
	std::atomic_size_t m_drop_count;
};





#include <iostream>

int main()
{
	using namespace std;
	using namespace std::chrono_literals;

	leaky_bucket_st bucket(250ms, 5, true);

	cout << bucket.count() << " " << bucket.full() << " " << bucket.empty() << endl;
	bucket.drain();
	cout << bucket.count() << " " << bucket.full() << " " << bucket.empty() << endl;

	try	{ bucket.leak_or_wait(bucket.capacity() + 1); }
	catch (exception& e) { cerr << e.what() << endl; }

	try { bucket.pour_or_wait(bucket.capacity() + 1); }
	catch (exception& e) { cerr << e.what() << endl << endl; }



	for (int i = 1; i <= 100; ++i)
	{
		if (bucket.full())
			break;

		bucket.pour_or_wait();
		cout << "pour " << i << "..." << endl;
	} cout << endl;

	while (!bucket.empty())
	{
		bucket.leak_or_wait();
		cout << "leak " << bucket.count() << "..." << endl;
	} cout << endl;

	cout << "sleeping..." << endl << endl;
	this_thread::sleep_for(1s);

	cout << "pouring..." << endl << endl;
	while (bucket.try_pour(3));

	while (!bucket.empty())
	{
		bucket.leak();
		cout << "leak " << bucket.count() << "..." << endl;
	}

	cout << "sleeping..." << endl << endl;
	this_thread::sleep_for(1s);

	cout << "refilling..." << endl << endl;
	bucket.refill();

	while (!bucket.empty())
	{
		bucket.leak();
		cout << "leak " << bucket.count() << "..." << endl;
	}
}
















/*#include <mutex>
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

int main()
{
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
*/
