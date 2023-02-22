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
	: m_time_per_token{ static_cast<std::uint64_t>(std::chrono::duration_cast<Resolution>(time_per_token).count()) }, m_token_capacity{ m_time_per_token * token_capacity }
	{
		if(std::chrono::duration_cast<std::chrono::nanoseconds>(time_per_token).count() < std::chrono::duration_cast<std::chrono::nanoseconds>(Resolution(1)).count())
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
	std::atomic_uint64_t m_time {};
	std::uint64_t m_time_per_token;
	std::uint64_t m_token_capacity;
};
