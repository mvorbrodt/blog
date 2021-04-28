#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <condition_variable>



class spinlock
{
public:
	void lock()
	{
		while(m_flag.test_and_set());
	}

	void unlock()
	{
		m_flag.clear();
	}

private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};



class manual_event
{
public:
	explicit manual_event(bool state = false) : m_signaled{ state } {}

	void signal()
	{
		{
			std::scoped_lock guard{ m_mutex };
			m_signaled = true;
		}
		m_cv.notify_all();
	}

	void wait()
	{
		std::unique_lock guard{ m_mutex };
		m_cv.wait(guard, [this] { return m_signaled; });
	}

private:
	bool m_signaled;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};



class auto_event
{
public:
	explicit auto_event(bool state = false) : m_signaled{ state } {}

	void signal()
	{
		{
			std::scoped_lock guard{ m_mutex };
			m_signaled = true;
		}
		m_cv.notify_one();
	}

	void wait()
	{
		std::unique_lock guard{ m_mutex };
		m_cv.wait(guard, [this] { return m_signaled; });
		m_signaled = false;
	}

private:
	bool m_signaled;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	// here you can use your spinlock instead:
	// spinlock m_mutex;
	// std::condition_variable_any m_cv;
};



class sempahore
{
public:
	explicit sempahore(unsigned int count = 0) : m_count{ count } {}

	void post()
	{
		{
			std::scoped_lock guard{ m_mutex };
			m_count = m_count + 1;
		}
		m_cv.notify_one();
	}

	void post(unsigned int count)
	{
		if(!count)
			throw std::invalid_argument("don't be silly!");

		{
			std::scoped_lock guard{ m_mutex };
			m_count = m_count + count;
		}
		m_cv.notify_all();
	}

	void wait()
	{
		std::unique_lock guard{ m_mutex };
		m_cv.wait(guard, [this] { return m_count > 0; });
		m_count = m_count - 1;
	}

private:
	unsigned int m_count;
	std::mutex m_mutex;
	std::condition_variable m_cv;
};



int main()
{
	using namespace std;
	using namespace std::chrono;
/*
	spinlock s1, s2;
	manual_event e1, e2;

	for(int i = 1; i <= 10; ++i)
	{
		thread([&](int x)
		{
			cout << "starting " << i << endl;
			e1.wait();
			cout << x << endl;
		}, i).detach();
		this_thread::sleep_for(10ms);
	}

	this_thread::sleep_for(1s);
	e1.signal();
	this_thread::sleep_for(1s);



	auto_event ae1, ae2;

	thread([&]
	{
		while(true)
		{
			ae1.wait();
			cout << "A";
			ae2.signal();
		}
	}).detach();

	thread([&]
	{
		while(true)
		{
			ae2.wait();
			cout << "B";
			ae1.signal();
		}
	}).detach();

	ae1.signal();

	this_thread::sleep_for(1ms);
*/


	sempahore sem1{ 2 };

	for(int i = 0; i < 10; ++i)
	{
		thread([&] (int x)
		{
			sem1.wait();
			cout << x;
			cout.flush();
		}, i).detach();
	}

	for(int i = 0; i < 8 / 2; ++i)
	{
		this_thread::sleep_for(1s);
		sem1.post(2);
	}

	this_thread::sleep_for(1s);

	std::mutex sm1, sm2, sm3;

	std::scoped_lock guard{ sm1, sm2, sm3 };
}
