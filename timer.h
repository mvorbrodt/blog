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
		auto start = std::chrono::high_resolution_clock::now();
		std::chrono::nanoseconds drift{0};
		while(!m_event.wait_for(m_tick - drift))
		{
			++m_ticks;
			for(auto& interval : m_intervals)
			{
				++interval.elapsed;
				if(interval.elapsed == interval.interval)
				{
					interval.proc();
					if(interval.event != nullptr)
						interval.event->signal();
					interval.elapsed = 0;
				}
			}
			auto end = std::chrono::high_resolution_clock::now();
			auto realDuration = end - start;
			auto fakeDuration = m_tick * m_ticks;
			drift = realDuration - fakeDuration;
		}
	})
	{}

	~timer()
	{
		m_event.signal();
		m_thread.join();
	}

	template<typename F, typename... Args>
	std::shared_ptr<manual_event> set_timeout(unsigned int timeout_in_ticks, F f, Args&&... args)
	{
		assert(timeout_in_ticks > 0);
		auto event = std::make_shared<manual_event>();
		auto proc = [=]() {
			if(event->wait_for(std::chrono::milliseconds(0))) return;
			f(args...);
		};
		m_intervals.insert(m_intervals.end(), { proc, timeout_in_ticks, 0, event });
		return event;
	}

	template<typename F, typename... Args>
	std::shared_ptr<manual_event> set_interval(unsigned int interval_in_ticks, F f, Args&&... args)
	{
		assert(interval_in_ticks > 0);
		auto event = std::make_shared<manual_event>();
		auto proc = [=]() {
			if(event->wait_for(std::chrono::milliseconds(0))) return;
			f(args...);
		};
		m_intervals.insert(m_intervals.end(), { proc, interval_in_ticks, 0, nullptr });
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
		unsigned int interval;
		unsigned int elapsed;
		std::shared_ptr<manual_event> event;
	};

	std::list<interval_ctx> m_intervals;
};

template<typename D, typename F, typename... Args>
std::shared_ptr<manual_event> set_timeout_thread(D d, F f, Args&&... args)
{
	auto event = std::make_shared<manual_event>();
	std::thread([=]()
	{
		if(event->wait_for(d)) return;
		f(args...);
	}).detach();
	return event;
}

template<typename D, typename F, typename... Args>
std::shared_ptr<manual_event> set_interval_thread(D d, F f, Args&&... args)
{
	auto event = std::make_shared<manual_event>();
	std::thread([=]()
	{
		while(true)
		{
			if(event->wait_for(d)) return;
			f(args...);
		}
	}).detach();
	return event;
}
