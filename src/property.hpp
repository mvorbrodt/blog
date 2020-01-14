#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <istream>
#include <ostream>
#include <cstddef>

template<typename T>
struct default_property_policy
{
protected:
	default_property_policy() = default;
	default_property_policy(const T& v) : m_value(v) {}
	default_property_policy(T&& v) : m_value(std::move(v)) {}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	default_property_policy(std::initializer_list<V> l) : m_value(l) {}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	default_property_policy(A&&... a) : m_value(std::forward<A>(a)...) {}

	decltype(auto) get() const { return(m_value); }
	decltype(auto) get() { return(m_value); }

	template<typename T2> void set(const T2& v) { m_value = v; }
	template<typename T2> void set(T2&& v) { m_value = std::forward<T2>(v); }

private:
	T m_value = T{};
};

template<typename T>
struct default_property_policy<T*>
{
};

template<typename T>
struct default_property_policy<T[]>
{
};

// PROPERTY TEMPLATE
template<typename T, typename P = default_property_policy<T>>
class property : public P
{
public:
	property() = default;

	property(const T& v) : P(v) {}
	property(T&& v) : P(std::move(v)) {}

	property(const property& p) : P(p.P::get()) {}
	property(property&& p) : P(std::move(p.P::get())) {}

	template<typename T2, typename P2> property(const property<T2, P2>& p) : P(p.P::get()) {}
	template<typename T2, typename P2> property(property<T2, P2>&& p) : P(std::move(p.P2::get())) {}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	property(std::initializer_list<V> l) : P(l) {}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	property(A&&... a) : P(std::forward<A>(a)...) {}

	property& operator = (const T& v)
	{
		P::set(v);
		fire_update_event();
		return *this;
	}

	property& operator = (T&& v)
	{
		P::set(std::move(v));
		fire_update_event();
		return *this;
	}

	template<typename U>
	property& operator = (const U& v)
	{
		P::set(v);
		fire_update_event();
		return *this;
	}

	template<typename U>
	property& operator = (U&& v)
	{
		P::set(std::forward<U>(v));
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
		{
			P::set(p.P::get());
			fire_update_event();
		}
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
		{
			P::set(std::move(p.P::get()));
			fire_update_event();
		}
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (const property<T2, P2>& p)
	{
		if(this != (decltype(this))&p)
		{
			P::set(p.P2::get());
			fire_update_event();
		}
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (property<T2, P2>&& p)
	{
		if(this != (decltype(this))&p)
		{
			P::set(std::move(p.P2::get()));
			fire_update_event();
		}
		return *this;
	}

	#ifndef PROPERTY_INC_DEC_OPERATOR
	#define PROPERTY_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		op P::get(); \
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
		P::get() op v; \
		fire_update_event(); \
		return *this; \
	} \
	template<typename U> \
	property& operator op (const U& v) \
	{ \
		P::get() op v; \
		fire_update_event(); \
		return *this; \
	} \
	property& operator op (const property& p) \
	{ \
		P::get() op p.P::get(); \
		fire_update_event(); \
		return *this; \
	} \
	template<typename T2, typename P2> \
	property& operator op (const property<T2, P2>& p) \
	{ \
		P::get() op p.P2::get(); \
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
	template<typename T2, typename P2, typename V> \
	friend auto operator op (const property<T2, P2>& lhs, const V& rhs) \
		-> property<decltype(std::declval<T2>() op std::declval<V>())>; \
	template<typename T2, typename P2, typename V> \
	friend auto operator op (const V& lhs, const property<T2, P2>& rhs) \
		-> property<decltype(std::declval<V>() op std::declval<T2>())>; \
	template<typename T2, typename P2, typename T3, typename P3> \
	friend auto operator op (const property<T2, P2>& lhs, const property<T3, P3>& rhs) \
		-> property<decltype(std::declval<T2>() op std::declval<T3>())>;
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

	T& get() { return P::get(); }
	const T& get() const { return P::get(); }

	explicit operator T& () { return P::get(); }
	operator const T& () const { return P::get(); }

	T& operator -> () { return P::get(); }
	const T& operator -> () const { return P::get(); }

	template<typename U> decltype(auto) operator [] (U&& i) { return(P::get()[std::forward<U>(i)]); }
	template<typename U> decltype(auto) operator [] (U&& i) const { return(P::get()[std::forward<U>(i)]); }

	template<typename F, typename... A>
	auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), P::get(), std::forward<A>(a)...); }

	using update_event_t = std::function<void(const property&)>;
	using update_event_list_t = std::vector<update_event_t>;

	void add_update_event(update_event_t&& proc) const
	{ m_update_events.push_back(std::forward<update_event_t>(proc)); }

	const update_event_list_t& get_update_events() const
	{ return m_update_events; }

	void clear_update_events() const
	{ m_update_events.clear(); }

private:
	template<typename, typename> friend class property;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// PROPERTY FRIEND OPERATORS
#ifndef PROPERTY_FRIEND_OPERATOR
#define PROPERTY_FRIEND_OPERATOR(op) \
template<typename T2, typename P2, typename V> \
auto operator op (const property<T2, P2>& lhs, const V& rhs) \
	-> property<decltype(std::declval<T2>() op std::declval<V>())> \
{ \
	return property(lhs.P2::get() op rhs); \
} \
template<typename T2, typename P2, typename V> \
auto operator op (const V& lhs, const property<T2, P2>& rhs) \
	-> property<decltype(std::declval<V>() op std::declval<T2>())> \
{ \
	return property(lhs op rhs.P2::get()); \
} \
template<typename T2, typename P2, typename T3, typename P3> \
auto operator op (const property<T2, P2>& lhs, const property<T3, P3>& rhs) \
	-> property<decltype(std::declval<T2>() op std::declval<T3>())> \
{ \
	return property(lhs.P2::get() op rhs.P3::get()); \
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

// PROPERTY I/O OPERATORS
template<typename T, typename P>
inline std::istream& operator >> (std::istream& os, property<T, P>& p)
{
	os >> p.get();
	return os;
}

template<typename T, typename P>
inline std::ostream& operator << (std::ostream& os, const property<T, P>& p)
{
	os << p.get();
	return os;
}



// POINTER SPECIALIZATION
template<typename T, typename P>
class property<T*, P> : public P
{
public:
	property() = default;

	property(T* const & v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}

	template<typename T2, typename P2> property(const property<T2*, P2>& p) : m_value(p.m_value) {}

	property& operator = (T* const & v)
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

	template<typename T2, typename P2>
	property& operator = (const property<T2*, P2>& p)
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

	T* get() { return m_value; }
	const T* get() const { return m_value; }

	explicit operator bool () const { return m_value != nullptr; }

	operator T* () { return m_value; }
	operator T* const () const { return m_value; }

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
	template<typename, typename> friend class property;

	T* m_value = nullptr;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// ARRAY SPECIALIZATION
template<typename T, typename P>
class property<T[], P> : public P
{
public:
	property() = default;

	property(T* const & v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}

	template<typename T2, typename P2> property(const property<T2[], P2>& p) : m_value(p.m_value) {}

	property& operator = (T* const & v)
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

	template<typename T2, typename P2>
	property& operator = (const property<T2[], P2>& p)
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

	T* get() { return m_value; }
	const T* get() const { return m_value; }

	explicit operator bool () const { return m_value != nullptr; }

	operator T* () { return m_value; }
	operator T* const () const { return m_value; }

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
	template<typename, typename> friend class property;

	T* m_value = nullptr;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// PROPERTY MAKER HELPERS
template<typename T, typename U>
inline auto make_property(std::initializer_list<U> l)
{
	using V = std::decay_t<T>;
	return property<T>(V(l));
}

template<typename T, typename P, typename U>
inline auto make_property(std::initializer_list<U> l)
{
	using V = std::decay_t<T>;
	return property<T, P>(V(l));
}

template<typename T, typename... A>
inline auto make_property(A&&... a)
{
	using V = std::decay_t<T>;
	return property<T>(V(std::forward<A>(a)...));
}

template<typename T, typename P, typename... A>
inline auto make_property(A&&... a)
{
	using V = std::decay_t<T>;
	return property<T, P>(V(std::forward<A>(a)...));
}
