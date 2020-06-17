#pragma once

#include <set>
#include <thread>
#include <chrono>
#include <memory>
#include <iterator>
#include <stdexcept>
#include <functional>
#include "event.hpp"

class timer
{
public:
	timer() : timer(std::chrono::seconds(1)) {}

	template<typename R, typename P>
	explicit timer(const std::chrono::duration<R, P>& tick)
	: m_tick(std::chrono::duration_cast<std::chrono::nanoseconds>(tick))
	{
		if(m_tick.count() <= 0)
		{
			throw std::invalid_argument("Invalid tick value: must be greater than zero!");
		}

		m_thread = std::make_unique<std::thread>([this]()
		{
			auto start = std::chrono::high_resolution_clock::now();
			std::chrono::nanoseconds drift{0};
			while(!m_event.wait_for(m_tick - drift))
			{
				++m_ticks;
				auto it = std::begin(m_events);
				auto end = std::end(m_events);
				while(it != end)
				{
					auto& event = *it;
					++event.elapsed;
					if(event.elapsed == event.ticks)
					{
						auto remove = event.proc();
						if(remove)
						{
							m_events.erase(it++);
							continue;
						}
						else
						{
							event.elapsed = 0;
						}
					}
					++it;
				}
				auto now = std::chrono::high_resolution_clock::now();
				auto realDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start);
				auto fakeDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(m_tick * m_ticks);
				drift = realDuration - fakeDuration;
			}
		});
	}

	~timer()
	{
		m_event.signal();
		m_thread->join();
	}

	template<typename R, typename P, typename F, typename... Args>
	[[nodiscard]] auto set_timeout(const std::chrono::duration<R, P>& timeout, F f, Args&&... args)
	{
		if(timeout.count() <= 0)
		{
			throw std::invalid_argument("Invalid timeout value: must be greater than zero!");
		}

		auto event = std::make_shared<manual_event>();
		auto proc = [=]()
		{
			if(event->wait_for(std::chrono::seconds(0)))
			{
				return true;
			}
			f(args...);
			return true;
		};
		m_events.insert({ event_ctx::NextSeqNum(), static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() / m_tick.count()), 0, proc, event });
		return event;
	}

	template<typename R, typename P, typename F, typename... Args>
	[[nodiscard]] auto set_interval(const std::chrono::duration<R, P>& interval, F f, Args&&... args)
	{
		if(interval.count() <= 0)
		{
			throw std::invalid_argument("Invalid interval value: must be greater than zero!");
		}

		auto event = std::make_shared<manual_event>();
		auto proc = [=]()
		{
			if(event->wait_for(std::chrono::seconds(0)))
			{
				return true;
			}
			f(args...);
			return false;
		};
		m_events.insert({ event_ctx::NextSeqNum(), static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count() / m_tick.count()), 0, proc, event });
		return event;
	}

private:
	std::chrono::nanoseconds m_tick;
	unsigned long long m_ticks = 0;
	manual_event m_event;
	using thread_ptr = std::unique_ptr<std::thread>;
	thread_ptr m_thread = nullptr;

	struct event_ctx
	{
		static inline unsigned long long NextSeqNum()
		{
			static unsigned long long kNextSeqNum = 0;
			return ++kNextSeqNum;
		}

		unsigned long long seq_num;
		unsigned long long ticks;
		mutable unsigned long long elapsed;
		std::function<bool(void)> proc;
		std::shared_ptr<manual_event> event;
	};

	struct event_ctx_less
	{
		constexpr bool operator () (const event_ctx& lhs, const event_ctx& rhs) const
		{
			return lhs.seq_num < rhs.seq_num;
		}
	};

	std::set<event_ctx, event_ctx_less> m_events;
};
