#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <queue>
#include <memory>
#include <vector>
#include <utility>
#include <functional>
#include <condition_variable>
#include <stdexcept>
#include <ctime>
#include <cstdlib>

template<typename T>
class unbounded_queue
{
public:
	unbounded_queue() = default;
	~unbounded_queue()
	{
		done();
	};

	void push(const T& item)
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_queue.push(item);
		}
		m_condition.notify_one();
	}

	void push(T&& item)
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_queue.push(std::move(item));
		}
		m_condition.notify_one();
	}

	bool pop(T& item)
	{
		std::unique_lock guard(m_queue_lock);
		m_condition.wait(guard, [&]() { return !m_queue.empty() || m_done; });
		if(m_done == true)
			return false;
		item = std::move(m_queue.front());
		m_queue.pop();
		return true;
	}

	std::size_t size() const
	{
		std::unique_lock guard(m_queue_lock);
		return m_queue.size();
	}

	bool empty() const
	{
		std::unique_lock guard(m_queue_lock);
		return m_queue.empty();
	}

	void done()
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_done = true;
		}
		m_condition.notify_all();
	}

private:
	using queue_t = std::queue<T>;
	queue_t m_queue;
	mutable std::mutex m_queue_lock;
	std::condition_variable m_condition;
	bool m_done = false;
};



template<typename T>
class bounded_queue
{
public:
	bounded_queue(std::size_t max_size) : m_max_size{ max_size }
	{
		if(!m_max_size)
			throw std::invalid_argument("bad queue max-size! must be non-zero!");
	}

	~bounded_queue()
	{
		done();
	};

	bool push(const T& item)
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_condition_push.wait(guard, [&]() { return m_queue.size() < m_max_size || m_done; });
			if(m_done)
				return false;
			m_queue.push(item);
		}
		m_condition_pop.notify_one();
		return true;
	}

	bool push(T&& item)
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_condition_push.wait(guard, [&]() { return m_queue.size() < m_max_size || m_done; });
			if(m_done)
				return false;
			m_queue.push(std::move(item));
		}
		m_condition_pop.notify_one();
		return true;
	}

	bool pop(T& item)
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_condition_pop.wait(guard, [&]() { return !m_queue.empty() || m_done; });
			if(m_done == true)
				return false;
			item = std::move(m_queue.front());
			m_queue.pop();
		}
		m_condition_push.notify_one();
		return true;
	}

	std::size_t size() const
	{
		std::unique_lock guard(m_queue_lock);
		return m_queue.size();
	}

	bool empty() const
	{
		std::unique_lock guard(m_queue_lock);
		return m_queue.empty();
	}

	void done()
	{
		{
			std::unique_lock guard(m_queue_lock);
			m_done = true;
		}
		m_condition_push.notify_all();
		m_condition_pop.notify_all();
	}

private:
	using queue_t = std::queue<T>;
	queue_t m_queue;
	mutable std::mutex m_queue_lock;
	std::condition_variable m_condition_push;
	std::condition_variable m_condition_pop;
	std::size_t m_max_size;
	bool m_done = false;
};



class thread_pool
{
public:
	explicit thread_pool(std::size_t thread_count = std::thread::hardware_concurrency())
	{
		if(!thread_count)
			throw std::invalid_argument("bad thread count! must be non-zero!");

		m_threads.reserve(thread_count);

		for(auto i = 0; i < thread_count; ++i)
		{
			m_threads.push_back(std::thread([this]()
			{
				while(true)
				{
					work_item_t work{};
					m_queue.pop(work);
					if(!work)
						break;
					work();
				}
			}));
		}
	}

	~thread_pool()
	{
		for(auto& t : m_threads)
			m_queue.push(work_item_t{});
		for(auto& t : m_threads)
			t.join();
	}

	using work_item_t = std::function<void(void)>;

	void do_work(work_item_t wi)
	{
		m_queue.push(std::move(wi));
	}

private:
	using threads_t = std::vector<std::thread>;
	threads_t m_threads;

	using queue_t = unbounded_queue<work_item_t>;
	queue_t m_queue;
};



int main()
{
	using namespace std;

	bounded_queue<int> q{ 5 };

	thread t{ [&]
	{
		while(true)
		{
			int i{};
			bool r = q.pop(i);
			if(!r)
				return;
			cout << "value: " << i << "\tsize: " << q.size() << endl;
		}
	}};

	for(int i = 1; i <= 50; ++i)
	{
		q.push(i);
	}

	this_thread::sleep_for(100ms);
	q.done();
	t.join();
}
