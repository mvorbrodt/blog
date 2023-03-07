#include <atomic>
#include <chrono>
#include <thread>
#include <cstddef>
#include <stdexcept>

// Single-Threaded Version of Token Bucket
class token_bucket_st
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;

	token_bucket_st(std::size_t tokens_per_second, std::size_t token_capacity, bool full = true)
	: token_bucket_st(std::chrono::duration_cast<duration>(std::chrono::seconds(1)) / tokens_per_second, token_capacity, full) {}

	token_bucket_st(duration time_per_token, std::size_t token_capacity, bool full = true)
	: m_time{ full ? time_point{} : clock::now() }, m_time_per_token{ time_per_token }, m_time_per_burst{ time_per_token * token_capacity }
	{
		if (!m_time_per_token.count()) throw std::invalid_argument("Invalid token rate!");
		if (!m_time_per_burst.count()) throw std::invalid_argument("Invalid token capacity!");
	}

	[[nodiscard]] bool try_consume(std::size_t tokens = 1, duration* time_needed = nullptr)
	{
		auto now = clock::now();
		auto delay = tokens * m_time_per_token;
		auto min_time = now - m_time_per_burst;
		auto new_time = min_time > m_time ? min_time + delay : m_time + delay;

		if (new_time > now)
		{
			if (time_needed != nullptr)
				*time_needed = new_time - now;

			return false;
		}

		m_time = new_time;

		return true;
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
	time_point m_time;
	duration m_time_per_token;
	duration m_time_per_burst;
};

// Multi-Threaded Version of Token Bucket
class token_bucket_mt
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;
	using atomic_time_point = std::atomic<time_point>;

	token_bucket_mt(std::size_t tokens_per_second, std::size_t token_capacity, bool full = true)
	: token_bucket_mt(std::chrono::duration_cast<duration>(std::chrono::seconds(1)) / tokens_per_second, token_capacity, full) {}

	token_bucket_mt(duration time_per_token, std::size_t token_capacity, bool full = true)
	: m_time{ full ? time_point{} : clock::now() }, m_time_per_token{ time_per_token }, m_time_per_burst{ time_per_token * token_capacity }
	{
		if(!m_time_per_token.count()) throw std::invalid_argument("Invalid token rate!");
		if(!m_time_per_burst.count()) throw std::invalid_argument("Invalid token capacity!");
	}

	token_bucket_mt(const token_bucket_mt& other)
	: m_time{ other.m_time.load(std::memory_order_relaxed) }, m_time_per_token{ other.m_time_per_token }, m_time_per_burst{ other.m_time_per_burst } {}

	token_bucket_mt& operator = (const token_bucket_mt& other)
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

// Default Token Bucket
using token_bucket = token_bucket_mt;
