#pragma once

#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <set>
#include <iterator>
#include <cassert>
#include "event.h"

class timer
{
public:
	template<typename T>
	explicit timer(T&& tick)
	: m_tick(std::chrono::duration_cast<std::chrono::nanoseconds>(tick)), m_thread([this]()
	{
		assert(m_tick.count() > 0);
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
	})
	{}

	~timer()
	{
		m_event.signal();
		m_thread.join();
	}

	template<typename T, typename F, typename... Args>
	auto set_timeout(T&& timeout, F f, Args&&... args)
	{
		assert(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() >= m_tick.count());
		auto event = std::make_shared<manual_event>();
		auto proc = [=]() {
			if(event->wait_for(std::chrono::seconds(0))) return true;
			f(args...);
			return true;
		};
		m_events.insert({ event_ctx::NextSeqNum(), static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() / m_tick.count()), 0, proc, event });
		return event;
	}

	template<typename T, typename F, typename... Args>
	auto set_interval(T&& interval, F f, Args&&... args)
	{
		assert(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count() >= m_tick.count());
		auto event = std::make_shared<manual_event>();
		auto proc = [=]() {
			if(event->wait_for(std::chrono::seconds(0))) return true;
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
	std::thread m_thread;

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
