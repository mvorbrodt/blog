#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <type_traits>
#include <initializer_list>
#include <cstddef>

template<typename T>
class property
{
public:
	template<typename U>
	friend class property;

	property() = default;

	property(const T& v) : m_value(v) {}
	property(T&& v) : m_value(std::move(v)) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(std::move(p.m_value)) {}

	template<typename U> property(const property<U>& p) : m_value(p.m_value) {}
	template<typename U> property(property<U>&& p) : m_value(std::move(p.m_value)) {}

	template<typename U> property(std::initializer_list<U> l) : m_value(l) {}

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
	template<typename U = T> \
	std::enable_if_t<std::is_arithmetic_v<U>, property&> \
	operator op () \
	{ \
		op m_value; \
		fire_update_event(); \
		return *this; \
	} \
	template<typename U = T> \
	std::enable_if_t<std::is_arithmetic_v<U>, property> \
	operator op (int) \
	{ \
		auto temp(*this); \
		operator op (); \
		return temp; \
	}
	PROPERTY_INC_DEC_OPERATOR(++);
	PROPERTY_INC_DEC_OPERATOR(--);
	#undef PROPERTY_INC_DEC_OPERATOR
	#endif

	#ifndef PROPERTY_ARITHMETIC_OPERATOR
	#define PROPERTY_ARITHMETIC_OPERATOR(op) \
	template<typename U = T> \
	std::enable_if_t<std::is_arithmetic_v<U>, property&> \
	operator op (const T& v) \
	{ \
		m_value op v; \
		fire_update_event(); \
		return *this; \
	}
	PROPERTY_ARITHMETIC_OPERATOR(+=);
	PROPERTY_ARITHMETIC_OPERATOR(-=);
	PROPERTY_ARITHMETIC_OPERATOR(*=);
	PROPERTY_ARITHMETIC_OPERATOR(/=);
	PROPERTY_ARITHMETIC_OPERATOR(&=);
	PROPERTY_ARITHMETIC_OPERATOR(|=);
	PROPERTY_ARITHMETIC_OPERATOR(^=);
	PROPERTY_ARITHMETIC_OPERATOR(%=);
	PROPERTY_ARITHMETIC_OPERATOR(>>=);
	PROPERTY_ARITHMETIC_OPERATOR(<<=);
	#undef PROPERTY_ARITHMETIC_OPERATOR
	#endif

	explicit operator T& () { return m_value; }
	operator const T& () const { return m_value; }

	template<typename U = T>
	std::enable_if_t<std::is_class_v<U>, T&>
	operator -> () { return m_value; }

	template<typename U = T>
	std::enable_if_t<std::is_class_v<U>, const T&>
	operator -> () const { return m_value; }

	template<typename F, typename... Args>
	auto invoke(F&& f, Args&&... args) -> std::invoke_result_t<F, T, Args...>
	{
		return std::invoke(std::forward<F>(f), m_value, std::forward<Args>(args)...);
	}

	using update_event_t = std::function<void(const property&)>;

	void add_update_event(update_event_t&& proc) const
	{
		m_update_events.push_back(std::forward<update_event_t>(proc));
	}

private:
	T m_value = T{};

	using update_event_list_t = std::vector<update_event_t>;
	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{
		for(auto& event : m_update_events)
			event(*this);
	}
};



template<typename T>
class property<T*>
{
public:
	template<typename U>
	friend class property;

	property() = default;

	property(const T*& v) : m_value(v) {}
	property(T*&& v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(p.m_value) {}

	template<typename U> property(const property<U*>& p) : m_value(p.m_value) {}
	template<typename U> property(property<U*>&& p) : m_value(p.m_value) {}

	property& operator = (const T*& v)
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
	operator const T* () const { return m_value; }

	T& operator * () { return *m_value; }
	const T& operator * () const { return *m_value; }

	T* operator -> () { return m_value; }
	const T* operator -> () const { return m_value; }

	T& operator [] (std::size_t i) { return m_value[i]; }
	const T& operator [] (std::size_t i) const { return m_value[i]; }

	template<typename F, typename... Args>
	auto invoke(F&& f, Args&&... args) -> std::invoke_result_t<F, T*, Args...>
	{
		return std::invoke(std::forward<F>(f), m_value, std::forward<Args>(args)...);
	}

	template<typename F, typename... Args>
	auto invoke(std::size_t i, F&& f, Args&&... args) -> std::invoke_result_t<F, T, Args...>
	{
		return std::invoke(std::forward<F>(f), m_value[i], std::forward<Args>(args)...);
	}

	using update_event_t = std::function<void(const property&)>;

	void add_update_event(update_event_t&& proc) const
	{
		m_update_events.push_back(std::forward<update_event_t>(proc));
	}

private:
	T* m_value = nullptr;

	using update_event_list_t = std::vector<update_event_t>;
	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{
		for(auto& event : m_update_events)
			event(*this);
	}
};



template<typename T>
class property<T[]>
{
public:
	template<typename U>
	friend class property;

	property() = default;

	property(const T*& v) : m_value(v) {}
	property(T*&& v) : m_value(v) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(p.m_value) {}

	template<typename U> property(const property<U[]>& p) : m_value(p.m_value) {}
	template<typename U> property(property<U[]>&& p) : m_value(p.m_value) {}

	property& operator = (const T*& v)
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
	operator const T* () const { return m_value; }

	T& operator * () { return *m_value; }
	const T& operator * () const { return *m_value; }

	T* operator -> () { return m_value; }
	const T* operator -> () const { return m_value; }

	T& operator [] (std::size_t i) { return m_value[i]; }
	const T& operator [] (std::size_t i) const { return m_value[i]; }

	template<typename F, typename... Args>
	auto invoke(std::size_t i, F&& f, Args&&... args) -> std::invoke_result_t<F, T, Args...>
	{
		return std::invoke(std::forward<F>(f), m_value[i], std::forward<Args>(args)...);
	}

	using update_event_t = std::function<void(const property&)>;

	void add_update_event(update_event_t&& proc) const
	{
		m_update_events.push_back(std::forward<update_event_t>(proc));
	}

private:
	T* m_value = nullptr;

	using update_event_list_t = std::vector<update_event_t>;
	mutable update_event_list_t m_update_events;

	void fire_update_event() const
	{
		for(auto& event : m_update_events)
			event(*this);
	}
};



template<typename T, typename U>
auto make_property(std::initializer_list<U> l)
{
	return property<T>(std::decay_t<T>{l});
}

template<typename T, typename... Args>
auto make_property(Args&&... args)
{
	return property<T>(std::decay_t<T>(std::forward<Args>(args)...));
}
