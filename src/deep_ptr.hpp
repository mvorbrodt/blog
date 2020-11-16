#include <utility>
#include <initializer_list>
#include <cstddef>

template<typename T>
class deep_ptr
{
public:
	using pointer = T*;
	using element_type = T;

	constexpr deep_ptr() noexcept = default;
	constexpr deep_ptr(std::nullptr_t) noexcept {}

	explicit deep_ptr(T* ptr) noexcept
	: m_ptr{ ptr } {}

	deep_ptr(const deep_ptr& other)
	: m_ptr{ !other ? nullptr : new T{ *other } } {}

	template<typename U> deep_ptr(const deep_ptr<U>& other)
	: m_ptr{ !other ? nullptr : new T{ *other } } {}

	deep_ptr(deep_ptr&& other) noexcept
	: m_ptr{ std::exchange(other.m_ptr, nullptr) } {}

	template<typename U> deep_ptr(deep_ptr<U>&& other) noexcept
	: m_ptr{ std::exchange(other.m_ptr, nullptr) } {}

	~deep_ptr() noexcept { delete m_ptr; }

	deep_ptr& operator = (deep_ptr other) noexcept
	{
		swap(other);
		return *this;
	}

	deep_ptr& operator = (std::nullptr_t) noexcept
	{
		reset();
		return *this;
	}

	explicit operator bool () const noexcept { return m_ptr != nullptr; }

	T& operator * () const noexcept { return *m_ptr; }

	T* operator -> () const noexcept { return m_ptr; }

	T* get() const noexcept { return m_ptr; }

	T* release() noexcept { return std::exchange(m_ptr, nullptr); }

	void reset(T* ptr = nullptr) noexcept { delete std::exchange(m_ptr, ptr); }

	void swap(deep_ptr& other) noexcept { std::swap(m_ptr, other.m_ptr); }

private:
	template<typename> friend class deep_ptr;

	T* m_ptr = nullptr;
};

template<typename T, typename V>
inline auto make_deep(std::initializer_list<V> l)
{
	using U = std::decay_t<T>;
	return deep_ptr<T>{ new U(l) };
}

template<typename T, typename... Args>
inline auto make_deep(Args&&... args)
{
	return deep_ptr<T>{ new T{ std::forward<Args>(args)... } };
}
