#include <memory>
#include <utility>
#include <compare>
#include <type_traits>
#include <initializer_list>
#include <cstddef>

template<typename T, typename D = std::default_delete<T>>
class deep_ptr
{
public:
	using pointer = T*;
	using element_type = T;
	using deleter_type = D;
	using reference_type = T&;

	constexpr deep_ptr() noexcept = default;
	constexpr deep_ptr(std::nullptr_t) noexcept {}

	explicit deep_ptr(pointer p) noexcept : m_p{ p } {}

	deep_ptr(pointer p, const deleter_type& d) : m_p{ p }, m_d{ d } {}

	deep_ptr(const deep_ptr& d) : m_p{ !d ? nullptr : new T{ *d } }, m_d{ d.m_d } {}
	deep_ptr(deep_ptr&& d) noexcept : m_p{ std::exchange(d.m_p, nullptr) }, m_d{ std::move(d.m_d) } {}

	template<typename U, typename V>
	deep_ptr(const deep_ptr<U, V>& d) : m_p{ !d ? nullptr : new T{ *d } }, m_d{ d.m_d } {}

	template<typename U, typename V>
	deep_ptr(deep_ptr<U, V>&& d) noexcept : m_p{ std::exchange(d.m_p, nullptr) }, m_d{ std::move(d.m_d) } {}

	~deep_ptr() noexcept { delete m_p; }

	deep_ptr& operator = (deep_ptr r) noexcept
	{
		swap(r);
		return *this;
	}

	template<typename U>
	friend bool operator == (const deep_ptr<T>& x, const deep_ptr<U>& y) noexcept { return x.m_p == y.m_p; }

	template<typename U>
	friend auto operator <=> (const deep_ptr<T>& x, const deep_ptr<U>& y) noexcept { return x.m_p <=> y.m_p; }

	explicit operator bool () const noexcept { return m_p != nullptr; }

	reference_type operator * () const { return *m_p; }

	pointer operator -> () const noexcept { return m_p; }

	pointer get() const noexcept { return m_p; }

	pointer release() noexcept { return std::exchange(m_p, nullptr); }

	void reset(pointer p = pointer()) noexcept { delete std::exchange(m_p, p); }

	void swap(deep_ptr& o) noexcept
	{
		std::swap(m_p, o.m_p);
		std::swap(m_d, o.m_d);
	}

private:
	template<typename, typename> friend class deep_ptr;

	pointer m_p = pointer();
	deleter_type m_d = deleter_type();
};

template<typename T, typename D>
inline void swap(deep_ptr<T, D>& x, deep_ptr<T, D>& y)
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
