#include <atomic>
#include <chrono>
#include <thread>
#include <stdexcept>

template<typename Resolution = std::chrono::nanoseconds>
class token_bucket
{
public:
	token_bucket(std::uint64_t tokens_per_second, std::uint64_t token_capacity)
	: token_bucket(std::chrono::duration_cast<Resolution>(std::chrono::seconds(1)) / tokens_per_second, token_capacity) {}

	template<typename Rep, typename Period>
	token_bucket(const std::chrono::duration<Rep, Period>& time_per_token, std::uint64_t token_capacity)
	: m_time_per_token{ static_cast<std::uint64_t>(std::chrono::duration_cast<Resolution>(time_per_token).count()) },
		m_token_capacity{ m_time_per_token * token_capacity }
	{
		if(std::chrono::duration_cast<std::chrono::nanoseconds>(time_per_token).count()
			< std::chrono::duration_cast<std::chrono::nanoseconds>(Resolution(1)).count())
			throw std::invalid_argument("Invalid resolution!");
		if(!time_per_token.count() || !token_capacity)
			throw std::invalid_argument("Invalid rate or capacity!");
	}

	token_bucket(const token_bucket& other)
	: m_time{ other.m_time.load(std::memory_order_relaxed) }, m_time_per_token{ other.m_time_per_token }, m_token_capacity{ other.m_token_capacity } {}

	token_bucket(token_bucket&& other)
	: m_time{ other.m_time.load(std::memory_order_relaxed) }, m_time_per_token{ other.m_time_per_token }, m_token_capacity{ other.m_token_capacity } {}

	token_bucket& operator = (token_bucket other)
	{
		m_time = other.m_time.load(std::memory_order_relaxed);
		m_time_per_token = other.m_time_per_token;
		m_token_capacity = other.m_token_capacity;
		return *this;
	}

	[[nodiscard]] bool try_consume(std::uint64_t tokens = 1, Resolution* time_needed = nullptr)
	{
		auto now = static_cast<std::uint64_t>(std::chrono::duration_cast<Resolution>(std::chrono::steady_clock::now().time_since_epoch()).count());
		auto delay = tokens * m_time_per_token;
		auto min_time = now - m_token_capacity;
		auto old_time = m_time.load(std::memory_order_relaxed);
		auto new_time = old_time;

		if (min_time > old_time)
			new_time = min_time;

		while (true)
		{
			new_time += delay;

			if (new_time > now)
			{
				if (time_needed != nullptr)
					*time_needed = Resolution(new_time - now);
	
				return false;
			}

			if (m_time.compare_exchange_weak(old_time, new_time, std::memory_order_relaxed, std::memory_order_relaxed))
				return true;

			new_time = old_time;
		}

		[[unlikely]] return false;
	}

	void consume(std::uint64_t tokens = 1)
	{
		while (!try_consume(tokens))
			std::this_thread::yield();
	}

	void wait(std::uint64_t tokens = 1)
	{
		Resolution time_needed{};
		while (!try_consume(tokens, &time_needed))
			std::this_thread::sleep_for(time_needed);
	}

private:
	std::atomic_uint64_t m_time = 0;
	std::uint64_t m_time_per_token;
	std::uint64_t m_token_capacity;
};



#include <iostream>
#include <iomanip>
#include <syncstream>
#include <vector>
#include <latch>
#include <barrier>

#define ss std::osyncstream(std::cout)

int main()
{
	using namespace std;
	using namespace std::chrono;

	auto count = 3;// thread::hardware_concurrency();
	auto run = atomic_bool{ true };
	auto total = atomic_uint64_t{};
	auto counts = vector<atomic_uint64_t>(count);
	auto threads = vector<jthread>(count);
	auto bucket = token_bucket(3us, 100);

	auto b2 = bucket;
	auto b3 = move(b2);
	bucket = b3;
	bucket = move(b2);
	
	auto fair_start = latch(count);
	auto fair_play = barrier(count);

	auto worker = [&](auto x)
	{
		fair_start.arrive_and_wait();
		while (run)
		{
			//fair_play.arrive_and_wait();
			bucket.consume();
			++total, ++counts[x];
		}
		fair_play.arrive_and_drop();
	};

	for (auto& t : threads)
		t = jthread(worker, --count);

	thread([&]
	{
		while (run)
		{
			this_thread::sleep_for(1.5s);
			for (auto& count : counts)
				ss << fixed << setprecision(5) << left << count << "\t" << (100.0 * count / total) << "%" << endl;
			ss << endl;
		}
	}).detach();

	cin.get();
	run = false;
}
