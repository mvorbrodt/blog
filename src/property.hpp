#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <istream>
#include <ostream>
#include <cstddef>



// AGGREGATE PROPERTY POLICY: CONTROLS ACCESS + EVENTS
// OVERLOADED FOR T, T*, T[], NO ACCESS RESTRICTIONS, FIRES EVENTS
template<typename T>
struct default_property_policy
{
	using update_event_t = std::function<void(void*)>;
	void add_update_event(const update_event_t& proc) { m_update_events.push_back(proc); }

protected:
	template<typename U> void validate(const U& v) const {}

	decltype(auto) get_value(const T& v) const { return(v); }
	decltype(auto) get_value(T& v) { return(v); }

	template<typename U> void set_value(T& v, const U& nv) { validate(nv); v = nv; fire_update_event(); }
	template<typename U> void set_value(T& v, U&& nv) { validate(nv); v = std::forward<U>(nv); fire_update_event(); }

	void fire_update_event() { for(auto& event : m_update_events) event(this); }

private:
	using update_event_list_t = std::vector<update_event_t>;
	update_event_list_t m_update_events;
};

template<typename TP>
struct default_property_policy<TP*>
{
	using update_event_t = std::function<void(void*)>;
	void add_update_event(const update_event_t& proc) { m_update_events.push_back(proc); }

protected:
	using T = TP*;

	void validate(const T& v) const {}

	decltype(auto) get_value(const T& v) const { return(v); }
	decltype(auto) get_value(T& v) { return(v); }

	void set_value(T& v, const T& nv) { validate(nv); v = nv; fire_update_event(); }
	void set_value(T& v, T&& nv) { validate(nv); v = nv; nv = nullptr; fire_update_event(); }

	void fire_update_event() { for(auto& event : m_update_events) event(this); }

private:
	using update_event_list_t = std::vector<update_event_t>;
	update_event_list_t m_update_events;
};

template<typename TA>
struct default_property_policy<TA[]> : default_property_policy<TA*> {};



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
	using PT = P<T>;

	property() { PT::validate(m_value); }

	property(const T& v) : m_value(v) { PT::validate(m_value); }
	property(T&& v) : m_value(std::move(v)) { PT::validate(m_value); }

	property(const property& p) : m_value(p.get()) { PT::validate(m_value); }
	property(property&& p) : m_value(std::move(p.get())) { PT::validate(m_value); }

	template<typename U> property(const property<U, P>& p) : m_value(p.get()) { PT::validate(m_value); }
	template<typename U> property(property<U, P>&& p) : m_value(std::move(p.get())) { PT::validate(m_value); }

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	property(std::initializer_list<V> l) : m_value(l) { PT::validate(m_value); }

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	property(A&&... a) : m_value(std::forward<A>(a)...) { PT::validate(m_value); }

	property& operator = (const T& v)
	{
		set(v);
		return *this;
	}

	property& operator = (T&& v)
	{
		set(std::move(v));
		return *this;
	}

	template<typename U>
	std::enable_if_t<!is_property_v<U>, property&>
	operator = (const U& v)
	{
		set(v);
		return *this;
	}

	template<typename U>
	std::enable_if_t<!is_property_v<U>, property&>
	operator = (U&& v)
	{
		set(std::forward<U>(v));
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
			set(p.get());
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
			set(std::move(p.get()));
		return *this;
	}

	template<typename U>
	property& operator = (const property<U, P>& p)
	{
		if(this != (decltype(this))&p)
			set(p.get());
		return *this;
	}

	template<typename U>
	property& operator = (property<U, P>&& p)
	{
		if(this != (decltype(this))&p)
			set(std::move(p.get()));
		return *this;
	}

	#ifndef PROPERTY_INC_DEC_OPERATOR
	#define PROPERTY_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		auto temp(m_value); \
		op temp; \
		set(std::move(temp)); \
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
		temp op v; \
		set(std::move(temp)); \
		return *this; \
	} \
	template<typename U> \
	property& operator op (const U& v) \
	{ \
		auto temp(m_value); \
		temp op v; \
		set(std::move(temp)); \
		return *this; \
	} \
	property& operator op (const property& p) \
	{ \
		auto temp(m_value); \
		temp op p.get(); \
		set(std::move(temp)); \
		return *this; \
	} \
	template<typename U> \
	property& operator op (const property<U, P>& p) \
	{ \
		auto temp(m_value); \
		temp op p.get(); \
		set(std::move(temp)); \
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

	decltype(auto) get() { return(PT::get_value(m_value)); }
	decltype(auto) get() const { return(PT::get_value(m_value)); }

	void set(const T& v) { PT::set_value(m_value, v); }
	void set(T&& v) { PT::set_value(m_value, std::move(v)); }

	explicit operator T& () { return get(); }
	operator const T& () const { return get(); }

	T& operator -> () { return get(); }
	const T& operator -> () const { return get(); }

	template<typename U> decltype(auto) operator [] (U&& i) { return(get()[std::forward<U>(i)]); }
	template<typename U> decltype(auto) operator [] (U&& i) const { return(get()[std::forward<U>(i)]); }

	template<typename F, typename... A>
	auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), get(), std::forward<A>(a)...); }

private:
	template<typename, template<typename> class> friend class property;

	T m_value = T{};
};



// PROPERTY FRIEND OPERATORS
#ifndef PROPERTY_FRIEND_OPERATOR
#define PROPERTY_FRIEND_OPERATOR(op) \
template<typename T2, template<typename> class P2, typename V> \
auto operator op (const property<T2, P2>& lhs, const V& rhs) \
	-> property<decltype(std::declval<T2>() op std::declval<V>()), P2> \
{ \
	return property(lhs.get() op rhs); \
} \
template<typename V, typename T2, template<typename> class P2> \
auto operator op (const V& lhs, const property<T2, P2>& rhs) \
	-> property<decltype(std::declval<V>() op std::declval<T2>()), P2> \
{ \
	return property(lhs op rhs.get()); \
} \
template<typename T2, typename T3, template<typename> class P2> \
auto operator op (const property<T2, P2>& lhs, const property<T3, P2>& rhs) \
	-> property<decltype(std::declval<T2>() op std::declval<T3>()), P2> \
{ \
	return property(lhs.get() op rhs.get()); \
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
	using PT = P<T*>;

	property() { PT::validate(m_value); }

	property(T* const & v) : m_value(v) { PT::validate(m_value); }
	property(T*&& v) : m_value(v) { PT::validate(m_value); v = nullptr; }

	property(const property& p) : m_value(p.get()) { PT::validate(m_value); }
	property(property&& p) : m_value(p.get()) { PT::validate(m_value); p.get() = nullptr; }

	template<typename U> property(const property<U*, P>& p) : m_value(p.get()) { PT::validate(m_value); }
	template<typename U> property(property<U*, P>&& p) : m_value(p.get()) { PT::validate(m_value); p.get() = nullptr; }

	property& operator = (T* const & v)
	{
		set(v);
		return *this;
	}
	
	property& operator = (T*&& v)
	{
		set(std::move(v));
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
			set(p.get());
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
			set(std::move(p.get()));
		return *this;
	}

	template<typename U>
	property& operator = (const property<U*, P>& p)
	{
		if(this != (decltype(this))&p)
			set(p.get());
		return *this;
	}

	template<typename U>
	property& operator = (property<U*, P>&& p)
	{
		if(this != (decltype(this))&p)
			set(std::move(p.get()));
		return *this;
	}

	#ifndef POINTER_INC_DEC_OPERATOR
	#define POINTER_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		auto temp(m_value); \
		op temp; \
		set(temp); \
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
		auto temp(m_value); \
		temp op v; \
		set(temp); \
		return *this; \
	}
	POINTER_ARITHMETIC_OPERATOR(+=);
	POINTER_ARITHMETIC_OPERATOR(-=);
	#undef POINTER_ARITHMETIC_OPERATOR
	#endif

	T* & get() { return PT::get_value(m_value); }
	T* const & get() const { return PT::get_value(m_value); }

	void set(T* const & v) { PT::set_value(m_value, v); }
	void set(T*&& v) { PT::set_value(m_value, std::move(v)); }

	explicit operator bool () const { return get() != nullptr; }

	operator T* () { return get(); }
	operator T* const () const { return get(); }

	T& operator * () { return *get(); }
	const T& operator * () const { return *get(); }

	T* operator -> () { return get(); }
	const T* operator -> () const { return get(); }

	T& operator [] (std::size_t i) { return get()[i]; }
	const T& operator [] (std::size_t i) const { return get()[i]; }

	template<typename F, typename... A>
	auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T*, A...>
	{ return std::invoke(std::forward<F>(f), get(), std::forward<A>(a)...); }

	template<typename F, typename... A>
	auto invoke(std::size_t i, F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), get()[i], std::forward<A>(a)...); }

private:
	template<typename, template<typename> class> friend class property;

	T* m_value = nullptr;
};



// ARRAY SPECIALIZATION
template<typename T, template<typename> class P>
class property<T[], P> : public P<T[]>
{
public:
	using PT = P<T[]>;

	property() { PT::validate(m_value); }

	property(T* const & v) : m_value(v) { PT::validate(m_value); }
	property(T*&& v) : m_value(v) { PT::validate(m_value); v = nullptr; }

	property(const property& p) : m_value(p.get()) { PT::validate(m_value); }
	property(property&& p) : m_value(p.get()) { PT::validate(m_value); p.get() = nullptr; }

	template<typename U> property(const property<U[], P>& p) : m_value(p.get()) { PT::validate(m_value); }
	template<typename U> property(property<U[], P>&& p) : m_value(p.get()) { PT::validate(m_value); p.get() = nullptr; }

	property& operator = (T* const & v)
	{
		set(v);
		return *this;
	}

	property& operator = (T*&& v)
	{
		set(std::move(v));
		return *this;
	}

	property& operator = (const property& p)
	{
		if(this != &p)
			set(p.get());
		return *this;
	}

	property& operator = (property&& p)
	{
		if(this != &p)
			set(std::move(p.get()));
		return *this;
	}

	template<typename U>
	property& operator = (const property<U[], P>& p)
	{
		if(this != (decltype(this))&p)
			set(p.get());
		return *this;
	}

	template<typename U>
	property& operator = (property<U[], P>&& p)
	{
		if(this != (decltype(this))&p)
			set(std::move(p.get()));
		return *this;
	}

	#ifndef POINTER_INC_DEC_OPERATOR
	#define POINTER_INC_DEC_OPERATOR(op) \
	property& operator op () \
	{ \
		auto temp(m_value); \
		op temp; \
		set(temp); \
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
		auto temp(m_value); \
		temp op v; \
		set(temp); \
		return *this; \
	}
	POINTER_ARITHMETIC_OPERATOR(+=);
	POINTER_ARITHMETIC_OPERATOR(-=);
	#undef POINTER_ARITHMETIC_OPERATOR
	#endif

	T* & get() { return PT::get_value(m_value); }
	T* const & get() const { return PT::get_value(m_value); }

	void set(T* const & v) { PT::set_value(m_value, v); }
	void set(T*&& v) { PT::set_value(m_value, std::move(v)); }

	explicit operator bool () const { return get() != nullptr; }

	operator T* () { return get(); }
	operator T* const () const { return get(); }

	T& operator * () { return *get(); }
	const T& operator * () const { return *get(); }

	T* operator -> () { return get(); }
	const T* operator -> () const { return get(); }

	T& operator [] (std::size_t i) { return get()[i]; }
	const T& operator [] (std::size_t i) const { return get()[i]; }

	template<typename F, typename... A>
	auto invoke(std::size_t i, F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), get()[i], std::forward<A>(a)...); }

private:
	template<typename, template<typename> class> friend class property;

	T* m_value = nullptr;
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



// PROPERTY CAST HELPERS
template<typename T>
inline property<T>* property_cast(void* p)
{
	return reinterpret_cast<property<T>*>(p);
}

template<typename T>
inline decltype(auto) property_cast_to_ref(void* p)
{
	return(*property_cast<T>(p));
}

template<typename T>
inline decltype(auto) property_cast_to_val(void* p)
{
	return(*property_cast_to_ref<T>(p));
}
