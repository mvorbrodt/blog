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
	timer(T&& tick)
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
					event.proc();
					if(event.remove_after_fire)
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
			if(realDuration > fakeDuration)
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
			if(event->wait_for(std::chrono::seconds(0))) return;
			f(args...);
		};
		m_events.insert({ event_ctx::kNextSeqNum++, proc,
			static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() / m_tick.count()), 0, event, true });
		return event;
	}

	template<typename T, typename F, typename... Args>
	auto set_interval(T&& interval, F f, Args&&... args)
	{
		assert(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count() >= m_tick.count());
		auto event = std::make_shared<manual_event>();
		auto proc = [=]() {
			if(event->wait_for(std::chrono::seconds(0))) return;
			f(args...);
		};
		m_events.insert({ event_ctx::kNextSeqNum++, proc,
			static_cast<unsigned long long>(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count() / m_tick.count()), 0, event, false });
		return event;
	}

private:
	std::chrono::nanoseconds m_tick;
	unsigned long long m_ticks = 0;
	manual_event m_event;
	std::thread m_thread;

	struct event_ctx
	{
		bool operator < (const event_ctx& rhs) const { return seq_num < rhs.seq_num; }
		static inline unsigned long long kNextSeqNum = 0;
		unsigned long long seq_num;
		std::function<void(void)> proc;
		unsigned long long ticks;
		mutable unsigned long long elapsed;
		std::shared_ptr<manual_event> event;
		bool remove_after_fire;
	};

	using set = std::set<event_ctx>;
	set m_events;
};
