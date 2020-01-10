#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <ostream>
#include <cstddef>

// PROPERTY TEMPLATE
template<typename T>
class property
{
public:
	property() = default;

	property(const T& v) : m_value(v) {}
	property(T&& v) : m_value(std::move(v)) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(std::move(p.m_value)) {}

	template<typename U> property(const property<U>& p) : m_value(p.m_value) {}
	template<typename U> property(property<U>&& p) : m_value(std::move(p.m_value)) {}

	template<typename U = T, typename V>
	property(std::initializer_list<V> l,
		std::enable_if_t<
			!std::is_scalar_v<U> &&
			std::is_constructible_v<U, std::initializer_list<V>>
		>* = nullptr) : m_value(l) {}

	template<typename... A> property(A&&... a) : m_value(std::forward<A>(a)...) {}

	property& operator = (const T& v)
	{
		m_value = v;
		fire_update_event();
		return *this;
	}

	property& operator = (T&& v)
	{
		m_value = std::move(v);
		fire_update_event();
		return *this;
	}

	template<typename U>
	property& operator = (const U& v)
	{
		m_value = v;
		fire_update_event();
		return *this;
	}

	template<typename U>
	property& operator = (U&& v)
	{
		m_value = std::move(v);
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
		{
			m_value = std::move(p.m_value);
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (const property<U>& p)
	{
		if(this != (decltype(this))&p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (property<U>&& p)
	{
		if(this != (decltype(this))&p)
		{
			m_value = std::move(p.m_value);
			fire_update_event();
		}
		return *this;
	}

	#ifndef PROPERTY_INC_DEC_OPERATOR
	#define PROPERTY_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		op m_value; \
		fire_update_event(); \
		return *this; \
	} \
	property operator op (int) \
	{ \
		auto temp(*this); \
		operator op (); \
		return temp; \
	}
	PROPERTY_INC_DEC_OPERATOR(++);
	PROPERTY_INC_DEC_OPERATOR(--);
	#undef PROPERTY_INC_DEC_OPERATOR
	#endif

	#ifndef PROPERTY_OPERATOR
	#define PROPERTY_OPERATOR(op) \
	property& operator op (const T& v) \
	{ \
		m_value op v; \
		fire_update_event(); \
		return *this; \
	} \
	template<typename U> \
	property& operator op (const U& v) \
	{ \
		m_value op v; \
		fire_update_event(); \
		return *this; \
	} \
	property& operator op (const property& p) \
	{ \
		m_value op p.m_value; \
		fire_update_event(); \
		return *this; \
	} \
	template<typename U > \
	property& operator op (const property<U>& p) \
	{ \
		m_value op p.m_value; \
		fire_update_event(); \
		return *this; \
	}
	PROPERTY_OPERATOR(+=);
	PROPERTY_OPERATOR(-=);
	PROPERTY_OPERATOR(*=);
	PROPERTY_OPERATOR(/=);
	PROPERTY_OPERATOR(&=);
	PROPERTY_OPERATOR(|=);
	PROPERTY_OPERATOR(^=);
	PROPERTY_OPERATOR(%=);
	PROPERTY_OPERATOR(>>=);
	PROPERTY_OPERATOR(<<=);
	#undef PROPERTY_OPERATOR
	#endif

	#ifndef PROPERTY_FRIEND_OPERATOR
	#define PROPERTY_FRIEND_OPERATOR(op) \
	template<typename U, typename V> \
	friend auto operator op (const property<U>& lhs, const V& rhs) \
		-> property<decltype(std::declval<U>() op std::declval<V>())>; \
	template<typename U, typename V> \
	friend auto operator op (const U& lhs, const property<V>& rhs) \
		-> property<decltype(std::declval<U>() op std::declval<V>())>; \
	template<typename U, typename V> \
	friend auto operator op (const property<U>& lhs, const property<V>& rhs) \
		-> property<decltype(std::declval<U>() op std::declval<V>())>;
	PROPERTY_FRIEND_OPERATOR(+);
	PROPERTY_FRIEND_OPERATOR(-);
	PROPERTY_FRIEND_OPERATOR(*);
	PROPERTY_FRIEND_OPERATOR(/);
	PROPERTY_FRIEND_OPERATOR(&);
	PROPERTY_FRIEND_OPERATOR(|);
	PROPERTY_FRIEND_OPERATOR(^);
	PROPERTY_FRIEND_OPERATOR(%);
	PROPERTY_FRIEND_OPERATOR(>>);
	PROPERTY_FRIEND_OPERATOR(<<);
	#undef PROPERTY_FRIEND_OPERATOR
	#endif

	explicit operator T& () { return m_value; }
	operator const T& () const { return m_value; }

	template<typename U = T>
	std::enable_if_t<std::is_class_v<U>, U&>
	operator -> () { return m_value; }

	template<typename U = T>
	std::enable_if_t<std::is_class_v<U>, const U&>
	operator -> () const { return m_value; }

	template<typename U = T, typename V>
	std::enable_if_t<std::is_class_v<U>, decltype(std::declval<U>()[V{}])&>
	operator [] (const V& i) { return m_value[i]; }

	template<typename U = T, typename V>
	std::enable_if_t<std::is_class_v<U>, const decltype(std::declval<U>()[V{}])&>
	operator [] (const V& i) const { return m_value[i]; }

	template<typename F, typename... A>
	auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), m_value, std::forward<A>(a)...); }

	using update_event_t = std::function<void(const property&)>;
	using update_event_list_t = std::vector<update_event_t>;

	void add_update_event(update_event_t&& proc) const
	{ m_update_events.push_back(std::forward<update_event_t>(proc)); }

	const update_event_list_t& get_update_events() const
	{ return m_update_events; }

	void clear_update_events() const
	{ m_update_events.clear(); }

private:
	template<typename U> friend class property;

	T m_value = T{};

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};

template<typename T>
inline auto& operator << (std::ostream& os, const property<T>& p)
{
	os << (T&)p;
	return os;
}

#ifndef PROPERTY_FRIEND_OPERATOR
#define PROPERTY_FRIEND_OPERATOR(op) \
template<typename U, typename V> \
inline auto operator op (const property<U>& lhs, const V& rhs) \
	-> property<decltype(std::declval<U>() op std::declval<V>())> \
{ \
	return property(lhs.m_value op rhs); \
} \
template<typename U, typename V> \
inline auto operator op (const U& lhs, const property<V>& rhs) \
	-> property<decltype(std::declval<U>() op std::declval<V>())> \
{ \
	return property(lhs op rhs.m_value); \
} \
template<typename U, typename V> \
inline auto operator op (const property<U>& lhs, const property<V>& rhs) \
	-> property<decltype(std::declval<U>() op std::declval<V>())> \
{ \
	return property(lhs.m_value op rhs.m_value); \
}
PROPERTY_FRIEND_OPERATOR(+);
PROPERTY_FRIEND_OPERATOR(-);
PROPERTY_FRIEND_OPERATOR(*);
PROPERTY_FRIEND_OPERATOR(/);
PROPERTY_FRIEND_OPERATOR(&);
PROPERTY_FRIEND_OPERATOR(|);
PROPERTY_FRIEND_OPERATOR(^);
PROPERTY_FRIEND_OPERATOR(%);
PROPERTY_FRIEND_OPERATOR(>>);
PROPERTY_FRIEND_OPERATOR(<<);
#undef PROPERTY_FRIEND_OPERATOR
#endif



// POINTER SPECIALIZATION
template<typename T>
class property<T*>
{
public:
	property() = default;

	property(T* const & v) : m_value(v) {}
	property(T* && v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(p.m_value) {}

	template<typename U> property(const property<U*>& p) : m_value(p.m_value) {}
	template<typename U> property(property<U*>&& p) : m_value(p.m_value) {}

	property& operator = (T* const & v)
	{
		m_value = v;
		fire_update_event();
		return *this;
	}

	property& operator = (T*&& v)
	{
		m_value = v;
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (const property<U*>& p)
	{
		if(this != (decltype(this))&p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (property<U*>&& p)
	{
		if(this != (decltype(this))&p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	#ifndef POINTER_INC_DEC_OPERATOR
	#define POINTER_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		op m_value; \
		fire_update_event(); \
		return *this; \
	} \
	property operator op (int) \
	{ \
		auto temp(*this); \
		operator op (); \
		return temp; \
	}
	POINTER_INC_DEC_OPERATOR(++);
	POINTER_INC_DEC_OPERATOR(--);
	#undef POINTER_INC_DEC_OPERATOR
	#endif

	#ifndef POINTER_ARITHMETIC_OPERATOR
	#define POINTER_ARITHMETIC_OPERATOR(op) \
	property& operator op (std::ptrdiff_t v) \
	{ \
		m_value op v; \
		fire_update_event(); \
		return *this; \
	}
	POINTER_ARITHMETIC_OPERATOR(+=);
	POINTER_ARITHMETIC_OPERATOR(-=);
	#undef POINTER_ARITHMETIC_OPERATOR
	#endif

	explicit operator bool () const { return m_value != nullptr; }

	explicit operator T* () { return m_value; }
	operator T* () const { return m_value; }

	T& operator * () { return *m_value; }
	const T& operator * () const { return *m_value; }

	T* operator -> () { return m_value; }
	const T* operator -> () const { return m_value; }

	T& operator [] (std::size_t i) { return m_value[i]; }
	const T& operator [] (std::size_t i) const { return m_value[i]; }

	template<typename F, typename... A>
	auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T*, A...>
	{ return std::invoke(std::forward<F>(f), m_value, std::forward<A>(a)...); }

	template<typename F, typename... A>
	auto invoke(std::size_t i, F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), m_value[i], std::forward<A>(a)...); }

	using update_event_t = std::function<void(const property&)>;
	using update_event_list_t = std::vector<update_event_t>;

	void add_update_event(update_event_t&& proc) const
	{ m_update_events.push_back(std::forward<update_event_t>(proc)); }

	const update_event_list_t& get_update_events() const
	{ return m_update_events; }

	void clear_update_events() const
	{ m_update_events.clear(); }

private:
	template<typename U> friend class property;

	T* m_value = nullptr;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// ARRAY SPECIALIZATION
template<typename T>
class property<T[]>
{
public:
	property() = default;

	property(T* const & v) : m_value(v) {}
	property(T* && v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(p.m_value) {}

	template<typename U> property(const property<U[]>& p) : m_value(p.m_value) {}
	template<typename U> property(property<U[]>&& p) : m_value(p.m_value) {}

	property& operator = (T* const & v)
	{
		m_value = v;
		fire_update_event();
		return *this;
	}

	property& operator = (T*&& v)
	{
		m_value = v;
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (const property<U[]>& p)
	{
		if(this != (decltype(this))&p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (property<U[]>&& p)
	{
		if(this != (decltype(this))&p)
		{
			m_value = p.m_value;
			fire_update_event();
		}
		return *this;
	}

	#ifndef POINTER_INC_DEC_OPERATOR
	#define POINTER_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		op m_value; \
		fire_update_event(); \
		return *this; \
	} \
	property operator op (int) \
	{ \
		auto temp(*this); \
		operator op (); \
		return temp; \
	}
	POINTER_INC_DEC_OPERATOR(++);
	POINTER_INC_DEC_OPERATOR(--);
	#undef POINTER_INC_DEC_OPERATOR
	#endif

	#ifndef POINTER_ARITHMETIC_OPERATOR
	#define POINTER_ARITHMETIC_OPERATOR(op) \
	property& operator op (std::ptrdiff_t v) \
	{ \
		m_value op v; \
		fire_update_event(); \
		return *this; \
	}
	POINTER_ARITHMETIC_OPERATOR(+=);
	POINTER_ARITHMETIC_OPERATOR(-=);
	#undef POINTER_ARITHMETIC_OPERATOR
	#endif

	explicit operator bool () const { return m_value != nullptr; }

	explicit operator T* () { return m_value; }
	operator T* () const { return m_value; }

	T& operator * () { return *m_value; }
	const T& operator * () const { return *m_value; }

	T* operator -> () { return m_value; }
	const T* operator -> () const { return m_value; }

	T& operator [] (std::size_t i) { return m_value[i]; }
	const T& operator [] (std::size_t i) const { return m_value[i]; }

	template<typename F, typename... A>
	auto invoke(std::size_t i, F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), m_value[i], std::forward<A>(a)...); }

	using update_event_t = std::function<void(const property&)>;
	using update_event_list_t = std::vector<update_event_t>;

	void add_update_event(update_event_t&& proc) const
	{ m_update_events.push_back(std::forward<update_event_t>(proc)); }

	const update_event_list_t& get_update_events() const
	{ return m_update_events; }

	void clear_update_events() const
	{ m_update_events.clear(); }

private:
	template<typename U> friend class property;

	T* m_value = nullptr;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// PROPERTY HELPERS
template<typename T, typename U>
inline auto make_property(std::initializer_list<U> l)
{
	using V = std::decay_t<T>;
	return property<T>(V(l));
}

template<typename T, typename... A>
inline auto make_property(A&&... a)
{
	using V = std::decay_t<T>;
	return property<T>(V(std::forward<A>(a)...));
}
