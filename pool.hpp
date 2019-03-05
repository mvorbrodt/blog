#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <memory>
#include <future>
#include <functional>
#include <type_traits>
#include <cassert>
#include "queue.h"

class simple_thread_pool
{
public:
	simple_thread_pool(
		unsigned int threads = std::thread::hardware_concurrency(),
		unsigned int queueDepth = std::thread::hardware_concurrency())
	: m_queue(queueDepth)
	{
		assert(queueDepth != 0);
		assert(threads != 0);
		for(unsigned int i = 0; i < threads; ++i)
			m_threads.emplace_back([this]()
			{
				while(true)
				{
					Proc f;
					m_queue.pop(f);
					if(f == nullptr)
					{
						m_queue.push(nullptr);
						break;
					}
					f();
				}
			});
	}

	~simple_thread_pool() noexcept
	{
		m_queue.push(nullptr);
		for(auto& thread : m_threads)
			thread.join();
	}

	template<typename F, typename... Args>
	void enqueue_work(F&& f, Args&&... args)
	{
		m_queue.push([=]() { f(args...); });
	}

	template<typename F, typename... Args>
	auto enqueue_task(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();
		m_queue.push([task](){ (*task)(); });
		return res;
	}

private:
	using Proc = std::function<void(void)>;
	blocking_queue<Proc> m_queue;

	using Threads = std::vector<std::thread>;
	Threads m_threads;
};





class thread_pool
{
public:
	explicit thread_pool(unsigned int threads = std::thread::hardware_concurrency())
	: m_queues(threads), m_count(threads)
	{
		assert(threads != 0);
		auto worker = [this](unsigned int i)
		{
			while(true)
			{
				Proc f;
				for(unsigned int n = 0; n < m_count; ++n)
					if(m_queues[(i + n) % m_count].try_pop(f)) break;
				if(!f && !m_queues[i].pop(f)) break;
				f();
			}
		};
		for(unsigned int i = 0; i < threads; ++i)
			m_threads.emplace_back(worker, i);
	}

	~thread_pool() noexcept
	{
		for(auto& queue : m_queues)
			queue.done();
		for(auto& thread : m_threads)
			thread.join();
	}

	template<typename F, typename... Args>
	void enqueue_work(F&& f, Args&&... args)
	{
		auto work = [f,args...]() { f(args...); };
		unsigned int i = m_index++;
		for(unsigned int n = 0; n < m_count * K; ++n)
			if(m_queues[(i + n) % m_count].try_push(work)) return;
		m_queues[i % m_count].push(work);
	}

	template<typename F, typename... Args>
	auto enqueue_task(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>
	{
		using return_type = typename std::result_of<F(Args...)>::type;
		auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
		std::future<return_type> res = task->get_future();

		auto work = [task](){ (*task)(); };
		unsigned int i = m_index++;
		for(unsigned int n = 0; n < m_count * K; ++n)
			if(m_queues[(i + n) % m_count].try_push(work)) return res;
		m_queues[i % m_count].push(work);

		return res;
	}

private:
	using Proc = std::function<void(void)>;
	using Queues = std::vector<simple_blocking_queue<Proc>>;
	Queues m_queues;

	using Threads = std::vector<std::thread>;
	Threads m_threads;

	const unsigned int m_count;
	std::atomic_uint m_index = 0;

	inline static const unsigned int K = 2;
};
