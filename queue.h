#pragma once

#include <mutex>
#include <chrono>
#include <utility>
#include <type_traits>
#include <cassert>
#include "semaphore.h"

template<typename T>
class blocking_queue
{
public:
	explicit blocking_queue(unsigned int size)
	: m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
	m_data((T*)operator new(size * sizeof(T))),
	m_openSlots(size), m_fullSlots(0)
	{
		assert(size != 0);
	}

	~blocking_queue() noexcept
	{
		while (m_count--)
		{
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
		}
		operator delete(m_data);
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value and
		std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T& item) noexcept
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (item);
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_copy_constructible<Q>::value and not
		std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T& item)
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (item);
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value and
		std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T&& item) noexcept
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (std::move(item));
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_constructible<Q>::value and not
		std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T&& item)
	{
		m_openSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (std::move(item));
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
	std::is_copy_constructible<Q>::value and
	std::is_nothrow_copy_constructible<Q>::value, bool>::type
	try_push(const T& item) noexcept
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (item);
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
	std::is_copy_constructible<Q>::value and not
	std::is_nothrow_copy_constructible<Q>::value, bool>::type
	try_push(const T& item)
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (item);
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
	std::is_move_constructible<Q>::value and
	std::is_nothrow_move_constructible<Q>::value, bool>::type
	try_push(T&& item) noexcept
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			new (m_data + m_pushIndex) T (std::move(item));
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
	std::is_move_constructible<Q>::value and not
	std::is_nothrow_move_constructible<Q>::value, bool>::type
	try_push(T&& item)
	{
		auto result = m_openSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				new (m_data + m_pushIndex) T (std::move(item));
			}
			catch (...)
			{
				m_openSlots.post();
				throw;
			}
			m_pushIndex = ++m_pushIndex % m_size;
			++m_count;
		}
		m_fullSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and
		std::is_nothrow_copy_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			item = m_data[m_popIndex];
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and not
		std::is_nothrow_copy_assignable<Q>::value, void>::type
	pop(T& item)
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = m_data[m_popIndex];
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and
		std::is_nothrow_move_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			item = std::move(m_data[m_popIndex]);
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and not
		std::is_nothrow_move_assignable<Q>::value, void>::type
	pop(T& item)
	{
		m_fullSlots.wait();
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = std::move(m_data[m_popIndex]);
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
	not std::is_move_assignable<Q>::value and
	std::is_nothrow_copy_assignable<Q>::value, bool>::type
	try_pop(T& item) noexcept
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			item = m_data[m_popIndex];
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
	not std::is_move_assignable<Q>::value and not
	std::is_nothrow_copy_assignable<Q>::value, bool>::type
	try_pop(T& item)
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = m_data[m_popIndex];
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
	std::is_move_assignable<Q>::value and
	std::is_nothrow_move_assignable<Q>::value, bool>::type
	try_pop(T& item) noexcept
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			item = std::move(m_data[m_popIndex]);
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	template<typename Q = T>
	typename std::enable_if<
	std::is_move_assignable<Q>::value and not
	std::is_nothrow_move_assignable<Q>::value, bool>::type
	try_pop(T& item)
	{
		auto result = m_fullSlots.wait_for(std::chrono::seconds(0));
		if(!result) return false;
		{
			std::scoped_lock lock(m_cs);
			try
			{
				item = std::move(m_data[m_popIndex]);
			}
			catch (...)
			{
				m_fullSlots.post();
				throw;
			}
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
			--m_count;
		}
		m_openSlots.post();
		return true;
	}

	bool empty() const
	{
		std::scoped_lock lock(m_cs);
		return m_count == 0;
	}

	bool full() const
	{
		std::scoped_lock lock(m_cs);
		return m_count == m_size;
	}

	bool size() const
	{
		std::scoped_lock lock(m_cs);
		return m_count;
	}

	bool capacity() const
	{
		return m_size;
	}

	T pop() noexcept(std::is_nothrow_invocable_r<void, decltype(&blocking_queue<T>::pop<T>), T&>::value)
	{
		T item;
		pop(item);
		return item;
	}

private:
	const unsigned int m_size;
	unsigned int m_pushIndex;
	unsigned int m_popIndex;
	unsigned int m_count;
	T* m_data;

	semaphore m_openSlots;
	semaphore m_fullSlots;
	std::mutex m_cs;
};
