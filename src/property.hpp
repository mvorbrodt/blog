#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <istream>
#include <ostream>
#include <cstddef>



// PROPERTY POLICY
template<typename T>
struct default_property_policy
{
protected:
	template<typename U> void validate(const U& v) const {}

	decltype(auto) get_value(const T& v) const { return(v); }
	decltype(auto) get_value(T& v) { return(v); }

	template<typename U> void set_value(T& v, const U& nv) { validate(nv); v = nv; }
	template<typename U> void set_value(T& v, U&& nv) { validate(nv); v = std::forward<U>(nv); }
};

template<typename T>
struct default_property_policy<T*>
{
};

template<typename T>
struct default_property_policy<T[]>
{
};



// PROPERTY TYPE TRAITS
template<typename T, template<typename> class P = default_property_policy>
class property;

template<typename>
struct is_property : std::false_type {};

template<typename T, template<typename> class P>
struct is_property<property<T, P>> : std::true_type {};

template<typename T>
inline constexpr bool is_property_v = is_property<std::decay_t<T>>::value;



// PROPERTY TEMPLATE
template<typename T, template<typename> class P>
class property : public P<T>
{
public:
	property() { P<T>::validate(m_value); }

	property(const T& v) : m_value(v) { P<T>::validate(m_value); }
	property(T&& v) : m_value(std::move(v)) { P<T>::validate(m_value); }

	property(const property& p) : m_value(p.P<T>::get_value(p.m_value)) { P<T>::validate(m_value); }
	property(property&& p) : m_value(std::move(p.P<T>::get_value(p.m_value))) { P<T>::validate(m_value); }

	template<typename U> property(const property<U, P>& p) : m_value(p.P<U>::get_value(p.m_value)) { P<T>::validate(m_value); }
	template<typename U> property(property<U, P>&& p) : m_value(std::move(p.P<U>::get_value(p.m_value))) { P<T>::validate(m_value); }

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	property(std::initializer_list<V> l) : m_value(l) { P<T>::validate(m_value); }

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	property(A&&... a) : m_value(std::forward<A>(a)...) { P<T>::validate(m_value); }

	property& operator = (const T& v)
	{
		P<T>::set_value(m_value, v);
		fire_update_event();
		return *this;
	}

	property& operator = (T&& v)
	{
		P<T>::set_value(m_value, std::move(v));
		fire_update_event();
		return *this;
	}

	template<typename U>
	std::enable_if_t<!is_property_v<U>, property&>
	operator = (const U& v)
	{
		P<T>::set_value(m_value, v);
		fire_update_event();
		return *this;
	}

	template<typename U>
	std::enable_if_t<!is_property_v<U>, property&>
	operator = (U&& v)
	{
		P<T>::set_value(m_value, std::forward<U>(v));
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
		{
			P<T>::set_value(m_value, p.P<T>::get_value(p.m_value));
			fire_update_event();
		}
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
		{
			P<T>::set_value(m_value, std::move(p.P<T>::get_value(p.m_value)));
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (const property<U, P>& p)
	{
		if(this != (decltype(this))&p)
		{
			P<T>::set_value(m_value, p.P<U>::get_value(p.m_value));
			fire_update_event();
		}
		return *this;
	}

	template<typename U>
	property& operator = (property<U, P>&& p)
	{
		if(this != (decltype(this))&p)
		{
			P<T>::set_value(m_value, std::move(p.P<U>::get_value(p.m_value)));
			fire_update_event();
		}
		return *this;
	}

	#ifndef PROPERTY_INC_DEC_OPERATOR
	#define PROPERTY_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		auto temp(m_value); \
		op m_value; \
		try \
		{ \
			P<T>::validate(m_value); \
		} \
		catch(...) \
		{ \
			m_value = std::move(temp); \
			throw; \
		} \
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
		auto temp(m_value); \
		m_value op v; \
		try \
		{ \
			P<T>::validate(m_value); \
		} \
		catch(...) \
		{ \
			m_value = std::move(temp); \
			throw; \
		} \
		fire_update_event(); \
		return *this; \
	} \
	template<typename U> \
	property& operator op (const U& v) \
	{ \
		auto temp(m_value); \
		m_value op v; \
		try \
		{ \
			P<T>::validate(m_value); \
		} \
		catch(...) \
		{ \
			m_value = std::move(temp); \
			throw; \
		} \
		fire_update_event(); \
		return *this; \
	} \
	property& operator op (const property& p) \
	{ \
		auto temp(m_value); \
		m_value op p.P<T>::get_value(p.m_value); \
		try \
		{ \
			P<T>::validate(m_value); \
		} \
		catch(...) \
		{ \
			m_value = std::move(temp); \
			throw; \
		} \
		fire_update_event(); \
		return *this; \
	} \
	template<typename U> \
	property& operator op (const property<U, P>& p) \
	{ \
		auto temp(m_value); \
		m_value op p.P<U>::get_value(p.m_value); \
		try \
		{ \
			P<T>::validate(m_value); \
		} \
		catch(...) \
		{ \
			m_value = std::move(temp); \
			throw; \
		} \
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
	template<typename T2, template<typename> class P2, typename V> \
	friend auto operator op (const property<T2, P2>& lhs, const V& rhs) \
		-> property<decltype(std::declval<T2>() op std::declval<V>()), P2>; \
	template<typename V, typename T2, template<typename> class P2> \
	friend auto operator op (const V& lhs, const property<T2, P2>& rhs) \
		-> property<decltype(std::declval<V>() op std::declval<T2>()), P2>; \
	template<typename T2, typename T3, template<typename> class P2> \
	friend auto operator op (const property<T2, P2>& lhs, const property<T3, P2>& rhs) \
		-> property<decltype(std::declval<T2>() op std::declval<T3>()), P2>;
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

	T& get() { return P<T>::get_value(m_value); }
	const T& get() const { return P<T>::get_value(m_value); }

	explicit operator T& () { return P<T>::get_value(m_value); }
	operator const T& () const { return P<T>::get_value(m_value); }

	T& operator -> () { return m_value; }
	const T& operator -> () const { return m_value; }

	template<typename U> decltype(auto) operator [] (U&& i) { return(m_value[std::forward<U>(i)]); }
	template<typename U> decltype(auto) operator [] (U&& i) const { return(m_value[std::forward<U>(i)]); }

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
	template<typename, template<typename> class> friend class property;

	T m_value = T{};

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// PROPERTY FRIEND OPERATORS
#ifndef PROPERTY_FRIEND_OPERATOR
#define PROPERTY_FRIEND_OPERATOR(op) \
template<typename T2, template<typename> class P2, typename V> \
auto operator op (const property<T2, P2>& lhs, const V& rhs) \
	-> property<decltype(std::declval<T2>() op std::declval<V>()), P2> \
{ \
	return property(lhs.P2<T2>::get_value(lhs.m_value) op rhs); \
} \
template<typename V, typename T2, template<typename> class P2> \
auto operator op (const V& lhs, const property<T2, P2>& rhs) \
	-> property<decltype(std::declval<V>() op std::declval<T2>()), P2> \
{ \
	return property(lhs op rhs.P2<T2>::get_value(rhs.m_value)); \
} \
template<typename T2, typename T3, template<typename> class P2> \
auto operator op (const property<T2, P2>& lhs, const property<T3, P2>& rhs) \
	-> property<decltype(std::declval<T2>() op std::declval<T3>()), P2> \
{ \
	return property(lhs.P2<T2>::get_value(lhs.m_value) op rhs.P2<T3>::get_value(rhs.m_value)); \
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
template<typename T, template<typename> class P>
inline std::istream& operator >> (std::istream& os, property<T, P>& p)
{
	os >> p.get();
	return os;
}

template<typename T, template<typename> class P>
inline std::ostream& operator << (std::ostream& os, const property<T, P>& p)
{
	os << p.get();
	return os;
}



// POINTER SPECIALIZATION
template<typename T, template<typename> class P>
class property<T*, P> : public P<T*>
{
public:
	property() = default;

	property(T* const & v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}

	template<typename U> property(const property<U*, P>& p) : m_value(p.m_value) {}

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

	template<typename U>
	property& operator = (const property<U*, P>& p)
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
	template<typename, template<typename> class> friend class property;

	T* m_value = nullptr;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// ARRAY SPECIALIZATION
template<typename T, template<typename> class P>
class property<T[], P> : public P<T[]>
{
public:
	property() = default;

	property(T* const & v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}

	template<typename U> property(const property<U[], P>& p) : m_value(p.m_value) {}

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

	template<typename U>
	property& operator = (const property<U[], P>& p)
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
	template<typename, template<typename> class> friend class property;

	T* m_value = nullptr;

	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{ for(auto& event : m_update_events) event(*this); }
};



// PROPERTY MAKER HELPERS
template<typename T, typename V>
inline auto make_property(std::initializer_list<V> l)
{
	using U = std::decay_t<T>;
	return property<T>(U(l));
}

template<typename T, template<typename> class P, typename V>
inline auto make_property(std::initializer_list<V> l)
{
	using U = std::decay_t<T>;
	return property<T, P>(U(l));
}

template<typename T, typename... A>
inline auto make_property(A&&... a)
{
	using U = std::decay_t<T>;
	return property<T>(U(std::forward<A>(a)...));
}

template<typename T, template<typename> class P, typename... A>
inline auto make_property(A&&... a)
{
	using U = std::decay_t<T>;
	return property<T, P>(U(std::forward<A>(a)...));
}
