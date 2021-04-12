#pragma once

#include <memory>
#include <utility>
#include <compare>
#include <type_traits>
#include <initializer_list>
#include <cstddef>

template<typename T>
struct default_cloner
{
	using pointer = T*;
	pointer operator () (pointer p) const
	{
		return (p ? new T{ *p } : nullptr);
	}
};

template<typename T, typename C = default_cloner<T>, typename D = std::default_delete<T>>
class deep_ptr
{
public:
	using pointer = T*;
	using element_type = T;
	using reference_type = T&;
	using cloner_type = C;
	using deleter_type = D;

	constexpr deep_ptr() noexcept = default;
	constexpr deep_ptr(std::nullptr_t) noexcept {}

	explicit deep_ptr(pointer p) noexcept : m_p{ p } {}

	deep_ptr(pointer p, cloner_type c) : m_c{ c }, m_p{ p } {}
	deep_ptr(pointer p, deleter_type d) : m_d{ d }, m_p{ p } {}
	deep_ptr(pointer p, cloner_type c, deleter_type d) : m_c{ c }, m_d{ d }, m_p{ p } {}

	deep_ptr(const deep_ptr& d) : m_c{ d.m_c }, m_d{ d.m_d }, m_p{ get_cloner()(d.m_p) } {}
	deep_ptr(deep_ptr&& d) noexcept : m_c{ std::move(d.m_c) }, m_d{ std::move(d.m_d) }, m_p{ std::exchange(d.m_p, nullptr) } {}

	template<typename U, typename V, typename W>
	deep_ptr(const deep_ptr<U, V, W>& d) : m_c{ d.m_c }, m_d{ d.m_d }, m_p{ get_cloner()(d.m_p) } {}

	template<typename U, typename V, typename W>
	deep_ptr(deep_ptr<U, V, W>&& d) noexcept : m_c{ std::move(d.m_c) }, m_d{ std::move(d.m_d) }, m_p{ std::exchange(d.m_p, nullptr) } {}

	~deep_ptr() noexcept { get_deleter()(get()); }

	deep_ptr& operator = (deep_ptr r) noexcept
	{
		swap(r);
		return *this;
	}

	friend bool operator == (const deep_ptr& x, const deep_ptr& y) noexcept { return x.m_p == y.m_p; }

	friend auto operator <=> (const deep_ptr& x, const deep_ptr& y) noexcept { return x.m_p <=> y.m_p; }

	explicit operator bool () const noexcept { return m_p != nullptr; }

	reference_type operator * () const { return *m_p; }

	pointer operator -> () const noexcept { return m_p; }

	pointer get() const noexcept { return m_p; }

	deleter_type& get_deleter() noexcept { return m_d; }

	const deleter_type& get_deleter() const noexcept { return m_d; }

	cloner_type& get_cloner() noexcept { return m_c; }

	const cloner_type& get_cloner() const noexcept { return m_c; }

	pointer release() noexcept { return std::exchange(m_p, nullptr); }

	void reset(pointer p = pointer()) noexcept { get_deleter()(std::exchange(m_p, p)); }

	void swap(deep_ptr& o) noexcept
	{
		std::swap(m_c, o.m_c);
		std::swap(m_d, o.m_d);
		std::swap(m_p, o.m_p);
	}

private:
	template<typename, typename, typename> friend class deep_ptr;

	cloner_type m_c = cloner_type();
	deleter_type m_d = deleter_type();
	pointer m_p = pointer();
};

template<typename T, typename C, typename D>
inline void swap(deep_ptr<T, C, D>& x, deep_ptr<T, C, D>& y)
{
	x.swap(y);
}

template<typename T, typename V>
inline auto make_deep(std::initializer_list<V> l)
{
	using U = std::decay_t<T>;
	return deep_ptr<T>(new U(l));
}

template<typename T, typename... A>
inline auto make_deep(A&&... a)
{
	return deep_ptr<T>(new T(std::forward<A>(a)...));
}
