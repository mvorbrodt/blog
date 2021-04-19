#include <iostream>
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
					work_item_ptr_t work{nullptr};
					{
						std::unique_lock guard(m_queue_lock);
						m_condition.wait(guard, [&]() { return !m_queue.empty(); });
						work = std::move(m_queue.front());
						m_queue.pop();
					}
					if(!work)
					{
						break;
					}
					(*work)();
				}
			}));
		}
	}

	~thread_pool()
	{
		{
			std::unique_lock guard(m_queue_lock);
			for(auto& t : m_threads)
				m_queue.push(work_item_ptr_t{nullptr});
		}
		for(auto& t : m_threads)
			t.join();
	}

	thread_pool(const thread_pool&) = delete;
	thread_pool(thread_pool&&) = delete;
	thread_pool& operator = (const thread_pool&) = delete;
	thread_pool& operator = (thread_pool&&) = delete;

	using work_item_t = std::function<void(void)>;

	void do_work(work_item_t wi)
	{
		auto work_item = std::make_unique<work_item_t>(std::move(wi));
		{
			std::unique_lock guard(m_queue_lock);
			m_queue.push(std::move(work_item));
		}
		m_condition.notify_one();
	}

private:
	using work_item_ptr_t = std::unique_ptr<work_item_t>;
	using work_queue_t = std::queue<work_item_ptr_t>;

	work_queue_t m_queue;
	std::mutex m_queue_lock;
	std::condition_variable m_condition;

	using threads_t = std::vector<std::thread>;
	threads_t m_threads;
};

int main()
{
	using namespace std;
	srand(time(NULL));
	mutex cout_guard;

	cout << "main thread ID: " << this_thread::get_id() << endl;

	thread_pool tp;

	for(auto i = 1; i <= 50; i++)
		tp.do_work([&, i = i]()
		{
			{
				unique_lock guard(cout_guard);
				cout << "doing work " << i << "..." << endl;
			}
			this_thread::sleep_for(chrono::milliseconds(rand() % 1000));
		});
}
