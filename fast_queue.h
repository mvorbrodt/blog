#pragma once

#include <atomic>
#include <utility>
#include <type_traits>
#include <cassert>
#include "semaphore.h"

template<typename T>
class fast_blocking_queue
{
public:
	explicit fast_blocking_queue(unsigned int size)
	: m_size(size), m_pushIndex(0), m_popIndex(0), m_count(0),
	m_data((T*)operator new(size * sizeof(T))),
	m_openSlots(size), m_fullSlots(0)
	{
		assert(size != 0);
	}

	~fast_blocking_queue() noexcept
	{
		while (m_count--)
		{
			m_data[m_popIndex].~T();
			m_popIndex = ++m_popIndex % m_size;
		}
		operator delete(m_data);
	}

	template<typename Q = T>
	typename std::enable_if<std::is_nothrow_copy_constructible<Q>::value, void>::type
	push(const T& item) noexcept
	{
		m_openSlots.wait();

		auto pushIndex = m_pushIndex.fetch_add(1);
		new (m_data + (pushIndex % m_size)) T (item);
		++m_count;

		auto expected = m_pushIndex.load();
		while(!m_pushIndex.compare_exchange_strong(expected, m_pushIndex % m_size))
			expected = m_pushIndex.load();

		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<std::is_nothrow_move_constructible<Q>::value, void>::type
	push(T&& item) noexcept
	{
		m_openSlots.wait();

		auto pushIndex = m_pushIndex.fetch_add(1);
		new (m_data + (pushIndex % m_size)) T (std::move(item));
		++m_count;

		auto expected = m_pushIndex.load();
		while(!m_pushIndex.compare_exchange_strong(expected, m_pushIndex % m_size))
			expected = m_pushIndex.load();

		m_fullSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		not std::is_move_assignable<Q>::value and
		std::is_nothrow_copy_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();

		auto popIndex = m_popIndex.fetch_add(1);
		item = m_data[popIndex % m_size];
		m_data[popIndex % m_size].~T();
		--m_count;

		auto expected = m_popIndex.load();
		while(!m_popIndex.compare_exchange_strong(expected, m_popIndex % m_size))
			expected = m_popIndex.load();

		m_openSlots.post();
	}

	template<typename Q = T>
	typename std::enable_if<
		std::is_move_assignable<Q>::value and
		std::is_nothrow_move_assignable<Q>::value, void>::type
	pop(T& item) noexcept
	{
		m_fullSlots.wait();

		auto popIndex = m_popIndex.fetch_add(1);
		item = std::move(m_data[popIndex % m_size]);
		m_data[popIndex % m_size].~T();
		--m_count;

		auto expected = m_popIndex.load();
		while(!m_popIndex.compare_exchange_strong(expected, m_popIndex % m_size))
			expected = m_popIndex.load();

		m_openSlots.post();
	}

	T pop() noexcept(std::is_nothrow_invocable_r<void, decltype(&fast_blocking_queue<T>::pop<T>), T&>::value)
	{
		T item;
		pop(item);
		return item;
	}

private:
	const unsigned int m_size;
	std::atomic_uint m_pushIndex;
	std::atomic_uint m_popIndex;
	std::atomic_uint m_count;
	T* m_data;

	fast_semaphore m_openSlots;
	fast_semaphore m_fullSlots;
};
