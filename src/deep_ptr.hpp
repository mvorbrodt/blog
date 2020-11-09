#include <utility>
#include <type_traits>
#include <initializer_list>
#include <cstddef>

#include <iostream>
using namespace std;

template<typename T>
class deep_ptr
{
public:
	using pointer = T*;
	using element_type = T;

	constexpr deep_ptr() noexcept /*= default;*/ { cout << "deep_ptr()\n"; }
	constexpr deep_ptr(std::nullptr_t) noexcept /*= default;*/ { cout << "deep_ptr(nullptr_t)\n"; }
	explicit deep_ptr(T* ptr) noexcept : m_ptr{ ptr } { cout << "deep_ptr(T*)\n"; }

	deep_ptr(const deep_ptr& other, std::enable_if_t<std::is_copy_constructible_v<T>>* = nullptr)
	: m_ptr{ !other ? nullptr : new T{ *other } } { cout << "deep_ptr(const deep_ptr&)\n"; }

	template<typename U, std::enable_if_t<std::is_constructible_v<T, U>>* = nullptr>
	deep_ptr(const deep_ptr<U>& other) : m_ptr{ !other ? nullptr : new T{ *other } } { cout << "deep_ptr(const deep_ptr<U>&)\n"; }

	deep_ptr(deep_ptr&& other) noexcept { swap(other); cout << "deep_ptr(deep_ptr&&)\n"; }

	template<typename U> deep_ptr(deep_ptr<U>&&) = delete;

	~deep_ptr() noexcept { delete get(); cout << "~deep_ptr()\n"; }

	deep_ptr& operator = (deep_ptr other) noexcept
	{
		cout << "operator = (deep_ptr)\n";
		swap(other);
		return *this;
	}
/*
	deep_ptr& operator = (std::nullptr_t) noexcept
	{
		cout << "operator = (nullptr_t)\n";
		reset();
		return *this;
	}
*/
	explicit operator bool() const noexcept { return get() != nullptr; }

	T& operator * () const { return *get(); }

	T* operator -> () const noexcept { return get(); }

	T* get() const noexcept { return m_ptr; }

	T* release() noexcept { return std::exchange(m_ptr, nullptr); }

	void reset(T* ptr = nullptr) noexcept { delete std::exchange(m_ptr, ptr); }

	void swap(deep_ptr& other) noexcept { std::swap(m_ptr, other.m_ptr); }

private:
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
