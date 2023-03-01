#include <atomic>
#include <chrono>
#include <thread>
#include <cstddef>
#include <stdexcept>

class token_bucket
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;
	using atomic_time_point = std::atomic<time_point>;

	token_bucket(std::size_t tokens_per_second, std::size_t token_capacity, bool full = true)
	: token_bucket(std::chrono::duration_cast<duration>(std::chrono::seconds(1)) / tokens_per_second, token_capacity, full) {}

	token_bucket(duration time_per_token, std::size_t token_capacity, bool full = true)
	: m_time{ full ? time_point{} : clock::now() }, m_time_per_token{ time_per_token }, m_time_per_burst{ time_per_token * token_capacity }
	{
		if(!m_time_per_token.count()) throw std::invalid_argument("Invalid token rate!");
		if(!m_time_per_burst.count()) throw std::invalid_argument("Invalid token capacity!");
	}

	token_bucket(const token_bucket& other)
	: m_time{ other.m_time.load(std::memory_order_relaxed) }, m_time_per_token{ other.m_time_per_token }, m_time_per_burst{ other.m_time_per_burst } {}

	token_bucket& operator = (const token_bucket& other)
	{
		m_time = other.m_time.load(std::memory_order_relaxed);
		m_time_per_token = other.m_time_per_token;
		m_time_per_burst = other.m_time_per_burst;
		return *this;
	}

	[[nodiscard]] bool try_consume(std::size_t tokens = 1, duration* time_needed = nullptr)
	{
		auto now = clock::now();
		auto delay = tokens * m_time_per_token;
		auto min_time = now - m_time_per_burst;
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
					*time_needed = new_time - now;

				return false;
			}

			if (m_time.compare_exchange_weak(old_time, new_time, std::memory_order_relaxed, std::memory_order_relaxed))
				return true;

			new_time = old_time;
		}

		[[unlikely]] return false;
	}

	void consume(std::size_t tokens = 1)
	{
		while (!try_consume(tokens))
			std::this_thread::yield();
	}

	void wait(std::size_t tokens = 1)
	{
		duration time_needed{};
		while (!try_consume(tokens, &time_needed))
			std::this_thread::sleep_for(time_needed);
	}

private:
	atomic_time_point m_time;
	duration m_time_per_token;
	duration m_time_per_burst;
};
