#pragma once

#include <thread>
#include <chrono>
#include <memory>
#include <functional>
#include <list>
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
			for(auto& interval : m_intervals)
			{
				++interval.elapsed;
				if(interval.elapsed == interval.ticks)
				{
					interval.proc();
					if(interval.event != nullptr)
						interval.event->signal();
					interval.elapsed = 0;
				}
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
			if(event->wait_for(std::chrono::seconds(0))) return;
			f(args...);
		};
		m_intervals.insert(m_intervals.end(), { proc,
			static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() / m_tick.count()), 0, event });
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
		m_intervals.insert(m_intervals.end(), { proc,
			static_cast<unsigned int>(std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count() / m_tick.count()), 0, nullptr });
		return event;
	}

private:
	std::chrono::nanoseconds m_tick;
	unsigned long long m_ticks = 0;
	manual_event m_event;
	std::thread m_thread;

	struct interval_ctx
	{
		std::function<void(void)> proc;
		unsigned int ticks;
		unsigned int elapsed;
		std::shared_ptr<manual_event> event;
	};

	std::list<interval_ctx> m_intervals;
};
