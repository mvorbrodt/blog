#include <mutex>
#include <shared_mutex>
#include <utility>
#include <type_traits>

namespace detail
{
	template<typename T, typename Lock = std::mutex>
	class locker
	{
	public:
		using mutex_type = Lock;

		locker(T& v, Lock& l)
		: m_value(v), m_guard(l) {}

		locker(const locker&) = delete;
		locker& operator = (const locker&) = delete;

		T* operator -> () { return &m_value; }
		const T* operator -> () const { return &m_value; }

	private:
		T& m_value;
		std::scoped_lock<Lock> m_guard;
	};

	template<typename T, typename Lock = std::mutex>
	class const_locker
	{
	public:
		using mutex_type = Lock;

		const_locker(T& v, Lock& l)
		: m_value(v), m_guard(l) {}

		const_locker(const const_locker&) = delete;
		const_locker& operator = (const const_locker&) = delete;

		const T* operator -> () const { return &m_value; }

	private:
		T& m_value;
		std::scoped_lock<Lock> m_guard;
	};



	template<typename T, typename Lock = std::shared_mutex>
	class shared_locker
	{
	public:
		using mutex_type = Lock;

		shared_locker(T& v, Lock& l)
		: m_value(v), m_guard(l) {}

		shared_locker(const shared_locker&) = delete;
		shared_locker& operator = (const shared_locker&) = delete;

		T* operator -> () { return &m_value; }
		const T* operator -> () const { return &m_value; }

	private:
		T& m_value;
		std::unique_lock<Lock> m_guard;
	};

	template<typename T, typename Lock = std::shared_mutex>
	class const_shared_locker
	{
	public:
		using mutex_type = Lock;

		const_shared_locker(T& v, Lock& l)
		: m_value(v), m_guard(l) {}

		const_shared_locker(const const_shared_locker&) = delete;
		const_shared_locker& operator = (const const_shared_locker&) = delete;

		const T* operator -> () const { return &m_value; }

	private:
		T& m_value;
		std::shared_lock<Lock> m_guard;
	};
}



template<typename T, typename Lock = std::mutex>
class synchronized
{
public:
	using value_type = T;
	using mutex_type = Lock;

	synchronized() = default;
	synchronized(const T& v) : m_value(v) {}
	synchronized(T&& v) : m_value(std::move(v)) {}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	synchronized(A&&... a) : m_value(std::forward<A>(a)...) {}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	synchronized(std::initializer_list<V> l) : m_value(l) {}

	synchronized(const synchronized&) = delete;
	synchronized& operator = (const synchronized&) = delete;

	detail::locker<T, Lock> operator -> ()
	{
		return detail::locker(m_value, m_lock);
	}

	detail::const_locker<const T, Lock> operator -> () const
	{
		return detail::const_locker(m_value, m_lock);
	}

private:
	T m_value = T();
	mutable Lock m_lock;
};



template<typename T, typename Lock = std::shared_mutex>
class shared_synchronized
{
public:
	using value_type = T;
	using mutex_type = Lock;

	shared_synchronized() = default;
	shared_synchronized(const T& v) : m_value(v) {}
	shared_synchronized(T&& v) : m_value(std::move(v)) {}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	shared_synchronized(A&&... a) : m_value(std::forward<A>(a)...) {}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	shared_synchronized(std::initializer_list<V> l) : m_value(l) {}

	shared_synchronized(const shared_synchronized&) = delete;
	shared_synchronized& operator = (const shared_synchronized&) = delete;

	detail::shared_locker<T, Lock> operator -> ()
	{
		return detail::shared_locker(m_value, m_lock);
	}

	detail::const_shared_locker<const T, Lock> operator -> () const
	{
		return detail::const_shared_locker(m_value, m_lock);
	}

private:
	T m_value = T();
	mutable Lock m_lock;
};
