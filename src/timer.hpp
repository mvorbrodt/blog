#pragma once

#include <list>
#include <mutex>
#include <tuple>
#include <atomic>
#include <thread>
#include <chrono>
#include <memory>
#include <iterator>
#include <stdexcept>
#include <functional>
#include <cstdint>
#include "event.hpp"

class timer
{
public:
	using clock = std::chrono::steady_clock;
	using duration = clock::duration;
	using time_point = clock::time_point;

	explicit timer(duration tick)
	: m_time_per_tick{ tick }, m_next_tick_time{ clock::now() }
	{
		if(m_time_per_tick.count() <= 0)
		{
			throw std::invalid_argument("Invalid tick value: must be greater than zero!");
		}

		m_tick_thread = std::thread([this]()
		{
			while(!m_tick_event.wait_until(m_next_tick_time += m_time_per_tick))
			{
				std::scoped_lock lock{ m_events_lock };

				auto event = std::begin(m_events);
				auto end = std::end(m_events);

				while(event != end)
				{
					if(event->tick(m_time_per_tick))
					{
						if(event->fire())
						{
							m_events.erase(event++);
							continue;
						}
						else
						{
							event->clear();
						}
					}

					++event;
				}
			}
		});
	}

	~timer()
	{
		m_tick_event.signal();
		m_tick_thread.join();
	}

	template<typename C, typename W>
	struct event_handle
	{
		void finish() { m_work_event.signal(); }
		void cancel() { m_cancel_event.signal(); }

		bool is_finished() { return m_work_event.wait_for(duration::zero()); }
		bool is_canceled() { return m_cancel_event.wait_for(duration::zero()); }

		void wait() { m_work_event.wait(); }

		bool wait_for(duration t) { return m_work_event.wait_for(t); }

		bool wait_until(time_point t) { return m_work_event.wait_until(t); }

	private:
		C m_cancel_event;
		W m_work_event;
	};

	using timeout_handle = event_handle<manual_event, manual_event>;
	using interval_handle = event_handle<manual_event, auto_event>;

	template<typename F, typename... Args>
	[[nodiscard]] auto set_timeout(duration timeout, F&& f, Args&&... args)
	{
		if(timeout.count() <= 0)
		{
			throw std::invalid_argument("Invalid timeout value: must be greater than zero!");
		}

		auto handle = std::make_shared<timeout_handle>();

		auto ctx = event_ctx(
			timeout,
			[=, p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() mutable
			{
				if(handle->is_canceled())
				{
					return true;
				}

				std::apply(p, t);
				handle->finish();

				return true;
			});

		{
			std::scoped_lock lock{ m_events_lock };
			m_events.push_back(std::move(ctx));
		}

		return handle;
	}

	template<typename F, typename... Args>
	[[nodiscard]] auto set_interval(duration interval, F&& f, Args&&... args)
	{
		if(interval.count() <= 0)
		{
			throw std::invalid_argument("Invalid interval value: must be greater than zero!");
		}

		auto handle = std::make_shared<interval_handle>();

		auto ctx = event_ctx(
			interval,
			[=, p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() mutable
			{
				if (handle->is_canceled())
				{
					return true;
				}

				std::apply(p, t);
				handle->finish();

				return false;
			});

		{
			std::scoped_lock lock{ m_events_lock };
			m_events.push_back(std::move(ctx));
		}

		return handle;
	}

private:
	duration m_time_per_tick;
	time_point m_next_tick_time;

	std::thread m_tick_thread;
	manual_event m_tick_event;

	struct event_ctx
	{
		using proc_t = std::function<bool(void)>;

		event_ctx(duration f, proc_t&& p)
		: m_frequency{ f }, m_proc{ std::move(p) } {}

		bool tick(duration t) { m_elapsed += t; return m_elapsed >= m_frequency; }
		bool fire() { return m_proc(); }
		void clear() { m_elapsed = duration::zero(); }

	private:
		duration m_frequency;
		duration m_elapsed = duration::zero();
		proc_t m_proc;
	};

	using event_list = std::list<event_ctx>;
	event_list m_events;
	std::recursive_mutex m_events_lock;
};
