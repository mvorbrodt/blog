#pragma once

#include <vector>
#include <string>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <istream>
#include <ostream>
#include <fstream>
#include <ios>
#include <cstddef>



// ===--------------------------------===
// ===---                          ---===
// ===---   FORWARD DECLARATIONS   ---===
// ===---                          ---===
// ===--------------------------------===

template<typename T>
struct basic_property_policy;

template<typename T, typename P = basic_property_policy<T>>
class basic_property;



// ===-----------------------------------------------------------------------===
// ===---                                                                 ---===
// ===---   DEFAULT PROPERTY POLICY: CONTROLS STORAGE + ACCESS + EVENTS   ---===
// ===---              NO ACCESS RESTRICTIONS, FIRES EVENTS               ---===
// ===---                                                                 ---===
// ===-----------------------------------------------------------------------===

template<typename T>
struct basic_property_policy
{
	basic_property_policy() { validate(m_value); }

	explicit basic_property_policy(const T& v) : m_value(v) { validate(m_value); }
	explicit basic_property_policy(T&& v) : m_value(std::move(v)) { validate(m_value); }

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	basic_property_policy(A&&... a) : m_value(std::forward<A>(a)...) { validate(m_value); }

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	basic_property_policy(std::initializer_list<V> l) : m_value(l) { validate(m_value); }

	using update_event_t = std::function<void(basic_property<T, basic_property_policy>*)>;
	using update_event_list_t = std::vector<update_event_t>;

	void add_update_event(update_event_t proc) { m_update_events.push_back(proc); }

protected:
	template<typename U> void validate(const U& v) const {}

	decltype(auto) get() const { return(m_value); }

	template<typename U> void set(U&& nv)
	{
		validate(nv);
		m_value = std::forward<U>(nv);
		fire_update_event();
	}

private:
	void fire_update_event()
	{
		for(auto& event : m_update_events)
			event((basic_property<T, basic_property_policy>*)this);
	}

	mutable T m_value = T{};
	update_event_list_t m_update_events;
};



// ===------------------------------------------------------===
// ===---                                                ---===
// ===---   STORAGE POLICY CLASS BASED PROPERTY POLICY   ---===
// ===---      NO ACCESS RESTRICTIONS, FIRES EVENTS      ---===
// ===---                                                ---===
// ===------------------------------------------------------===

template<typename T, typename P>
struct storage_property_policy
{
	explicit storage_property_policy(P p) : m_storage_provider(p)
	{
		validate(m_value);
		m_storage_provider.save(m_value);
	}

	storage_property_policy(P p, const T& v) : m_value(v), m_storage_provider(p)
	{
		validate(m_value);
		m_storage_provider.save(m_value);
	}

	storage_property_policy(P p, T&& v) : m_value(std::move(v)), m_storage_provider(p)
	{
		validate(m_value);
		m_storage_provider.save(m_value);
	}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<T, A...>>* = nullptr>
	storage_property_policy(P p, A&&... a) : m_value(std::forward<A>(a)...), m_storage_provider(p)
	{
		validate(m_value);
		m_storage_provider.save(m_value);
	}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<T, std::initializer_list<V>>>* = nullptr>
	storage_property_policy(P p, std::initializer_list<V> l) : m_value(l), m_storage_provider(p)
	{
		validate(m_value);
		m_storage_provider.save(m_value);
	}

	using update_event_t = std::function<void(basic_property<T, storage_property_policy>*)>;
	using update_event_list_t = std::vector<update_event_t>;

	void add_update_event(update_event_t proc) { m_update_events.push_back(proc); }

protected:
	void validate(const T& v) const {}

	decltype(auto) get() const
	{
		m_storage_provider.load(m_value);
		return(m_value);
	}

	template<typename U> void set(U&& nv)
	{
		validate(nv);
		m_value = std::forward<U>(nv);
		m_storage_provider.save(m_value);
		fire_update_event();
	}

private:
	void fire_update_event()
	{
		for(auto& event : m_update_events)
			event((basic_property<T, storage_property_policy>*)this);
	}

	mutable T m_value = T{};
	P m_storage_provider;
	update_event_list_t m_update_events;
};



// ===---------------------------------------------===
// ===---                                       ---===
// ===---   DISK BASED PROPERTY VALUE STORAGE   ---===
// ===---                                       ---===
// ===---------------------------------------------===

struct file_storage_provider
{
	explicit file_storage_provider(const char* path) : m_path(path) {}

	template<typename T>
		std::enable_if_t<std::is_trivial_v<T> && std::is_standard_layout_v<T>, void>
	save(const T& v) const
	{
		using out_char_t = typename std::ofstream::char_type;

		std::ofstream ofs(m_path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
		ofs.exceptions(std::ios::failbit | std::ios::badbit);

		ofs.write(reinterpret_cast<const out_char_t*>(&v), sizeof(v));
		ofs.flush();
		ofs.close();
	}

	template<typename T>
		std::enable_if_t<std::is_trivial_v<T> && std::is_standard_layout_v<T>, void>
	load(T& v) const
	{
		using in_char_t = typename std::ifstream::char_type;

		std::ifstream ifs(m_path, std::ios_base::binary | std::ios_base::in);
		ifs.exceptions(std::ios::failbit | std::ios::badbit);

		ifs.read(reinterpret_cast<in_char_t*>(&v), sizeof(v));
		ifs.close();
	}

	template<class CharT, class Traits, class Allocator>
	void save(const std::basic_string<CharT, Traits, Allocator>& v) const
	{
		using str_t = std::basic_string<CharT, Traits, Allocator>;
		using str_char_t = typename str_t::value_type;
		using out_char_t = typename std::ofstream::char_type;

		std::ofstream ofs(m_path, std::ios_base::binary | std::ios_base::out | std::ios_base::trunc);
		ofs.exceptions(std::ios::failbit | std::ios::badbit);

		typename str_t::size_type bytes = v.length() * sizeof(str_char_t);

		ofs.write(reinterpret_cast<const out_char_t*>(v.data()), bytes);
		ofs.flush();
		ofs.close();
	}

	template<class CharT, class Traits, class Allocator>
	void load(std::basic_string<CharT, Traits, Allocator>& v) const
	{
		using str_t = std::basic_string<CharT, Traits, Allocator>;
		using str_char_t = typename str_t::value_type;
		using in_char_t = typename std::ifstream::char_type;

		std::ifstream ifs(m_path, std::ios_base::binary | std::ios_base::in | std::ios::ate);
		ifs.exceptions(std::ios::failbit | std::ios::badbit);

		typename str_t::size_type bytes = ifs.tellg();
		str_t temp(bytes / sizeof(str_char_t), str_char_t{});
		ifs.seekg(0);

		ifs.read(reinterpret_cast<in_char_t*>(temp.data()), bytes);
		ifs.close();

		v = std::move(temp);
	}

private:
	std::string m_path;
};



// ===--------------------------------===
// ===---                          ---===
// ===---   PROPERTY TYPE TRAITS   ---===
// ===---                          ---===
// ===--------------------------------===

template<typename>
struct is_property : std::false_type {};

template<typename T, typename P>
struct is_property<basic_property<T, P>> : std::true_type {};

template<typename T>
inline constexpr bool is_property_v = is_property<std::decay_t<T>>::value;



// ===--------------------------------------------===
// ===---                                      ---===
// ===---   PROPERTY TEMPLATE IMPLEMENTATION   ---===
// ===---                                      ---===
// ===--------------------------------------------===

template<typename T, typename P>
class basic_property : public P
{
public:
	basic_property() = default;

	basic_property(const T& v) : P(v) {}
	basic_property(T&& v) :  P(std::move(v)) {}

	basic_property(const basic_property& p) : P(p.get()) {}
	basic_property(basic_property&& p) : P(std::move(p.get())) {}

	template<typename U, typename P2> basic_property(const basic_property<U, P2>& p) : P(p.get()) {}
	template<typename U, typename P2> basic_property(basic_property<U, P2>&& p) : P(std::move(p.get())) {}

	template<typename... A, std::enable_if_t<
		std::is_constructible_v<P, A...>>* = nullptr>
	basic_property(A&&... a) : P(std::forward<A>(a)...) {}

	template<typename V, std::enable_if_t<
		std::is_constructible_v<P, std::initializer_list<V>>>* = nullptr>
	basic_property(std::initializer_list<V> l) : P(std::move(l)) {}

	template<typename U>
		std::enable_if_t<!is_property_v<U>, basic_property&>
	operator = (U&& v)
	{
		set(std::forward<U>(v));
		return *this;
	}

	basic_property& operator = (const basic_property& p)
	{
		if(this != &p)
			set(p.get());
		return *this;
	}

	basic_property& operator = (basic_property&& p)
	{
		if(this != &p)
			set(std::move(p.get()));
		return *this;
	}

	template<typename U, typename V>
	basic_property& operator = (const basic_property<U, V>& p)
	{
		if(this != (decltype(this))&p)
			set(p.get());
		return *this;
	}

	template<typename U, typename V>
	basic_property& operator = (basic_property<U, V>&& p)
	{
		if(this != (decltype(this))&p)
			set(std::move(p.get()));
		return *this;
	}

	#ifdef PROPERTY_OPERATOR
	#error "PROPERTY_OPERATOR should not be defined!"
	#endif
	#define PROPERTY_OPERATOR(op) \
	basic_property& operator op () \
	{ \
		auto temp(get()); \
		op temp; \
		set(std::move(temp)); \
		return *this; \
	} \
	basic_property operator op (int) \
	{ \
		auto temp(*this); \
		operator op (); \
		return temp; \
	}
	PROPERTY_OPERATOR(++)
	PROPERTY_OPERATOR(--)
	#undef PROPERTY_OPERATOR

	#ifdef PROPERTY_OPERATOR
	#error "PROPERTY_OPERATOR should not be defined!"
	#endif
	#define PROPERTY_OPERATOR(op) \
	basic_property& operator op (const T& v) \
	{ \
		auto temp(get()); \
		temp op v; \
		set(std::move(temp)); \
		return *this; \
	} \
	template<typename U> \
		std::enable_if_t<!is_property_v<U>, basic_property&> \
	operator op (const U& v) \
	{ \
		auto temp(get()); \
		temp op v; \
		set(std::move(temp)); \
		return *this; \
	} \
	template<typename U, typename V> \
	basic_property& operator op (const basic_property<U, V>& p) \
	{ \
		auto temp(get()); \
		temp op p.get(); \
		set(std::move(temp)); \
		return *this; \
	}
	PROPERTY_OPERATOR(+=)
	PROPERTY_OPERATOR(-=)
	PROPERTY_OPERATOR(*=)
	PROPERTY_OPERATOR(/=)
	PROPERTY_OPERATOR(&=)
	PROPERTY_OPERATOR(|=)
	PROPERTY_OPERATOR(^=)
	PROPERTY_OPERATOR(%=)
	PROPERTY_OPERATOR(>>=)
	PROPERTY_OPERATOR(<<=)
	#undef PROPERTY_OPERATOR

	decltype(auto) get() { return(P::get()); }
	decltype(auto) get() const { return(P::get()); }

	void set(const T& v) { P::set(v); }
	void set(T&& v) { P::set(std::move(v)); }

	explicit operator T& () { return get(); }
	operator const T& () const { return get(); }

	T& operator -> () { return get(); }
	const T& operator -> () const { return get(); }

	template<typename U> decltype(auto) operator [] (U&& i) { return(get()[std::forward<U>(i)]); }
	template<typename U> decltype(auto) operator [] (U&& i) const { return(get()[std::forward<U>(i)]); }

	template<typename F, typename... A>
	auto invoke(F&& f, A&&... a) -> std::invoke_result_t<F, T, A...>
	{ return std::invoke(std::forward<F>(f), get(), std::forward<A>(a)...); }
};



// ===-----------------------------------------===
// ===---                                   ---===
// ===---   PROPERTY ARYTHMETIC OPERATORS   ---===
// ===---                                   ---===
// ===-----------------------------------------===

#ifdef PROPERTY_OPERATOR
#error "PROPERTY_OPERATOR should not be defined!"
#endif
#define PROPERTY_OPERATOR(op) \
template<typename T2, typename P2, typename V, \
	std::enable_if_t<!is_property_v<V>, int> = 0> \
auto operator op (const basic_property<T2, P2>& lhs, const V& rhs) \
	-> basic_property<decltype(std::declval<T2>() op std::declval<V>()), P2> \
{ \
	return basic_property(lhs.get() op rhs); \
} \
template<typename V, typename T2, typename P2, \
	std::enable_if_t<!is_property_v<V>, int> = 0> \
auto operator op (const V& lhs, const basic_property<T2, P2>& rhs) \
	-> basic_property<decltype(std::declval<V>() op std::declval<T2>()), P2> \
{ \
	return basic_property(lhs op rhs.get()); \
} \
template<typename T2, typename T3, typename P2> \
auto operator op (const basic_property<T2, P2>& lhs, const basic_property<T3, P2>& rhs) \
	-> basic_property<decltype(std::declval<T2>() op std::declval<T3>()), P2> \
{ \
	return basic_property(lhs.get() op rhs.get()); \
}
PROPERTY_OPERATOR(+)
PROPERTY_OPERATOR(-)
PROPERTY_OPERATOR(*)
PROPERTY_OPERATOR(/)
PROPERTY_OPERATOR(&)
PROPERTY_OPERATOR(|)
PROPERTY_OPERATOR(^)
PROPERTY_OPERATOR(%)
PROPERTY_OPERATOR(>>)
PROPERTY_OPERATOR(<<)
#undef PROPERTY_OPERATOR



// ===-------------------------------------------===
// ===---                                     ---===
// ===---   PROPERTY INPUT/OUTPUT OPERATORS   ---===
// ===---                                     ---===
// ===-------------------------------------------===

template<class CharT, class Traits, typename T, typename P>
inline auto& operator >> (std::basic_istream<CharT, Traits>& is, basic_property<T, P>& p)
{
	is >> p.get();
	return is;
}

template<class CharT, class Traits, typename T, typename P>
inline auto& operator << (std::basic_ostream<CharT, Traits>& os, const basic_property<T, P>& p)
{
	os << p.get();
	return os;
}



// ===-------------------------------------------===
// ===---                                     ---===
// ===---   PROPERTY MAKER HELPER FUNCTIONS   ---===
// ===---                                     ---===
// ===-------------------------------------------===

template<typename T, typename V>
inline auto make_property(std::initializer_list<V> l)
{
	using U = std::decay_t<T>;
	return basic_property<T>(U(l));
}

template<typename T, typename P, typename V>
inline auto make_property(std::initializer_list<V> l)
{
	using U = std::decay_t<T>;
	return basic_property<T, P>(U(l));
}

template<typename T, typename... A>
inline auto make_property(A&&... a)
{
	using U = std::decay_t<T>;
	return basic_property<T>(U(std::forward<A>(a)...));
}

template<typename T, typename P, typename... A>
inline auto make_property(A&&... a)
{
	using U = std::decay_t<T>;
	return basic_property<T, P>(U(std::forward<A>(a)...));
}



// ===---------------------------------===
// ===---                           ---===
// ===---   PROPERTY TYPE ALIASES   ---===
// ===---                           ---===
// ===---------------------------------===

template<typename T>
using property = basic_property<T, basic_property_policy<T>>;

template<typename T>
using file_property = basic_property<T, storage_property_policy<T, file_storage_provider>>;



// ===-------------------===
// ===---             ---===
// ===---   THE END   ---===
// ===---             ---===
// ===-------------------===
