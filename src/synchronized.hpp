#include <mutex>
#include <utility>
#include <type_traits>

namespace detail
{
	template<typename U>
	class locker
	{
	public:
		locker(U& v, std::recursive_mutex& l) noexcept
		: m_value(v), m_lock(l) { m_lock.lock(); }

		locker(const locker&) = delete;
		locker& operator = (const locker&) = delete;

		~locker() noexcept { m_lock.unlock(); }

		U* operator -> () noexcept { return &m_value; }

	private:
		U& m_value;
		std::recursive_mutex& m_lock;
	};
}

template<typename T>
class synchronized
{
public:
	synchronized() = default;
	synchronized(const T& v) : m_value(v) {}
	synchronized(T&& v) : m_value(std::move(v)) {}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	synchronized(A&&... a) : m_value(std::forward<A>(a)...) {}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	synchronized(std::initializer_list<V> l) : m_value(l) {}

	synchronized(const synchronized& other) : m_value(other.m_value) {}
	synchronized(synchronized&& other) : m_value(std::move(other.m_value)) {}

	synchronized& operator = (synchronized other) noexcept
	{
		std::swap(m_value, other.m_value);
		return *this;
	}

	detail::locker<T> operator -> () noexcept
	{
		return detail::locker(m_value, m_lock);
	}

private:
	T m_value;
	mutable std::recursive_mutex m_lock;
};
