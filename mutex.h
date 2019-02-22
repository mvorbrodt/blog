/*
 * fast_mutex and rw_fast_mutex implemented by Chris Thomasson
 */
#pragma once

#include <atomic>
#include <climits>
#include "event.h"
#include "semaphore.h"

class spinlock_mutex
{
public:
	void lock()
	{
		while(flag.test_and_set(std::memory_order_acquire));
	}

	void unlock()
	{
		flag.clear(std::memory_order_release);
	}

private:
	std::atomic_flag flag = ATOMIC_FLAG_INIT;
};

class fast_mutex
{
public:
	fast_mutex() : m_state(0) {}

	void lock()
	{
		if (m_state.exchange(1, std::memory_order_acquire))
			while (m_state.exchange(2, std::memory_order_acquire))
				m_waitset.wait();
	}

	void unlock()
	{
		if (m_state.exchange(0, std::memory_order_release) == 2)
			m_waitset.signal();
	}

private:
	std::atomic_uint m_state;
	auto_event m_waitset;
};

class rw_fast_mutex
{
public:
	rw_fast_mutex()
	: m_wrstate(1), m_count(INT_MAX), m_rdwake(0),
	m_rdwset(0), m_wrwset(0), m_wrmtx(0) {}

	void read_lock()
	{
		if (m_count.fetch_add(-1, std::memory_order_acquire) < 1)
			m_rdwset.wait();
	}

	void read_unlock()
	{
		if (m_count.fetch_add(1, std::memory_order_release) < 0)
			if (m_rdwake.fetch_add(-1, std::memory_order_acq_rel) == 1)
				m_wrwset.post();
	}

	void write_lock()
	{
		if (m_wrstate.fetch_sub(1, std::memory_order_acquire) < 1)
			m_wrmtx.wait();
		int count = m_count.fetch_add(-INT_MAX, std::memory_order_acquire);
		if (count < INT_MAX)
		{
			int rdwake = m_rdwake.fetch_add(INT_MAX - count, std::memory_order_acquire);
			if (rdwake + INT_MAX - count)
				m_wrwset.wait();
		}
	}

	void write_unlock()
	{
		int count = m_count.fetch_add(INT_MAX, std::memory_order_release);
		if (count < 0)
			m_rdwset.post(-count);
		if (m_wrstate.fetch_add(1, std::memory_order_release) < 0)
			m_wrmtx.post();
	}

private:
	std::atomic_int m_wrstate;
	std::atomic_int m_count;
	std::atomic_int m_rdwake;

	semaphore m_rdwset;
	semaphore m_wrwset;
	semaphore m_wrmtx;
};
