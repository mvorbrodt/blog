#pragma once

#include <set>
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
	template<typename R, typename P>
	explicit timer(const std::chrono::duration<R, P>& tick)
	: m_tick{ std::chrono::duration_cast<std::chrono::nanoseconds>(tick) }
	{
		if(m_tick.count() <= 0)
		{
			throw std::invalid_argument("Invalid tick value: must be greater than zero!");
		}

		m_tick_thread = std::make_unique<std::thread>([this]()
		{
			std::chrono::nanoseconds drift{ 0 };
			auto start = std::chrono::high_resolution_clock::now();

			while(!m_tick_event.wait_for(m_tick - drift))
			{
				++m_ticks;

				std::scoped_lock lock{ m_events_lock };

				auto it = std::begin(m_events);
				auto end = std::end(m_events);

				while(it != end)
				{
					auto& e = *it;

					if(++e->elapsed == e->ticks)
					{
						auto remove = e->proc();
						if(remove)
						{
							m_events.erase(it++);
							continue;
						}
						else
						{
							e->elapsed = 0;
						}
					}

					++it;
				}

				auto now = std::chrono::high_resolution_clock::now();
				auto real_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(now - start);
				auto fake_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(m_tick * m_ticks);

				drift = real_duration - fake_duration;
			}
		});
	}

	~timer()
	{
		m_tick_event.signal();
		m_tick_thread->join();
	}

	using manual_event_ptr = std::shared_ptr<manual_event>;
	using auto_event_ptr = std::shared_ptr<auto_event>;

	template<typename C, typename W>
	struct event_handle
	{
		event_handle(C cancel_event, W work_event)
		: m_cancel_event{ cancel_event }, m_work_event{ work_event } {}

		void cancel() { m_cancel_event->signal(); }

		void wait() { m_work_event->wait(); }

		template<typename R, typename P>
		bool wait_for(const std::chrono::duration<R, P>& t) { return m_work_event->wait_for(t); }

		template<typename R, typename P>
		bool wait_until(const std::chrono::duration<R, P>& t) { return m_work_event->wait_until(t); }

	private:
		C m_cancel_event;
		W m_work_event;
	};

	template<typename R, typename P, typename F, typename... Args>
	[[nodiscard]] auto set_timeout(const std::chrono::duration<R, P>& timeout, F&& f, Args&&... args)
	{
		if(timeout.count() <= 0)
		{
			throw std::invalid_argument("Invalid timeout value: must be greater than zero!");
		}

		auto cancel_event = std::make_shared<manual_event>();
		auto work_event = std::make_shared<manual_event>();
		auto ctx = std::make_shared<event_ctx>(
			std::chrono::duration_cast<std::chrono::nanoseconds>(timeout).count() / m_tick.count(),
			[=, p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() mutable
			{
				if(cancel_event->wait_for(std::chrono::seconds{0}))
				{
					return true;
				}

				std::apply(p, t);
				work_event->signal();

				return true;
			});

		{
			std::scoped_lock lock{ m_events_lock };
			m_events.insert(ctx);
		}

		return event_handle{ cancel_event, work_event };
	}

	template<typename R, typename P, typename F, typename... Args>
	[[nodiscard]] auto set_interval(const std::chrono::duration<R, P>& interval, F&& f, Args&&... args)
	{
		if(interval.count() <= 0)
		{
			throw std::invalid_argument("Invalid interval value: must be greater than zero!");
		}

		auto cancel_event = std::make_shared<manual_event>();
		auto work_event = std::make_shared<auto_event>();
		auto ctx = std::make_shared<event_ctx>(
			std::chrono::duration_cast<std::chrono::nanoseconds>(interval).count() / m_tick.count(),
			[=, p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() mutable
			{
				if(cancel_event->wait_for(std::chrono::seconds{0}))
				{
					return true;
				}

				std::apply(p, t);
				work_event->signal();

				return false;
			});

		{
			std::scoped_lock lock{ m_events_lock };
			m_events.insert(ctx);
		}

		return event_handle{ cancel_event, work_event };
	}

private:
	std::chrono::nanoseconds m_tick;
	std::int64_t m_ticks = 0;

	using thread_ptr = std::unique_ptr<std::thread>;
	thread_ptr m_tick_thread;
	manual_event m_tick_event;

	struct event_ctx
	{
		using proc_t = std::function<bool(void)>;

		event_ctx(std::int64_t t, proc_t&& p)
		: ticks{ t }, proc{ std::move(p) } {}

		std::int32_t seq_num = s_next.fetch_add(1);
		std::int64_t ticks;
		std::int64_t elapsed = 0;
		proc_t proc;

	private:
		static inline std::atomic_int32_t s_next = 0;
	};

	using event_ctx_ptr = std::shared_ptr<event_ctx>;

	struct event_ctx_less
	{
		constexpr bool operator () (const event_ctx_ptr& lhs, const event_ctx_ptr& rhs) const
		{
			return lhs->seq_num < rhs->seq_num;
		}
	};

	using event_list = std::set<event_ctx_ptr, event_ctx_less>;
	event_list m_events;
	std::mutex m_events_lock;
};
