#pragma once

#include <vector>
#include <utility>
#include <functional>
#include <type_traits>

template<typename T>
class property
{
public:
	property() = default;

	property(const T& v) : m_value(v) {}
	property(T&& v) : m_value(std::move(v)) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(std::move(p.m_value)) {}

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

	#ifndef PROPERTY_OPERATOR
	#define PROPERTY_OPERATOR(op) \
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
	PROPERTY_OPERATOR(++)
	PROPERTY_OPERATOR(--)
	#undef PROPERTY_OPERATOR
	#endif

	#ifndef PROPERTY_OPERATOR
	#define PROPERTY_OPERATOR(op) \
	template<typename Q> \
	property& operator op (const Q& nv) \
	{ \
		m_value op nv; \
		fire_update_event(); \
		return *this; \
	}
	PROPERTY_OPERATOR(+=)
	PROPERTY_OPERATOR(-=)
	PROPERTY_OPERATOR(*=)
	PROPERTY_OPERATOR(/=)
	PROPERTY_OPERATOR(%=)
	PROPERTY_OPERATOR(^=)
	PROPERTY_OPERATOR(&=)
	PROPERTY_OPERATOR(|=)
	PROPERTY_OPERATOR(>>=)
	PROPERTY_OPERATOR(<<=)
	#undef PROPERTY_OPERATOR
	#endif

	explicit operator T& () { return m_value; }
	operator const T& () const { return m_value; }

	template<typename Q = T>
	std::enable_if_t<std::is_pointer_v<Q> || std::is_class_v<Q>, T&>
	operator -> () { return m_value; }

	template<typename Q = T>
	std::enable_if_t<std::is_pointer_v<Q> || std::is_class_v<Q>, T&>
	operator -> () const { return m_value; }

	template<typename Q = T>
	std::enable_if_t<std::is_pointer_v<Q>, std::remove_pointer_t<T>&>
	operator * () { return *m_value; }

	template<typename Q = T>
	std::enable_if_t<std::is_pointer_v<Q>, const std::remove_pointer_t<T>&>
	operator * () const { return *m_value; }

	template<typename Q = T>
	std::enable_if_t<std::is_pointer_v<Q>, std::remove_pointer_t<T>&>
	operator [] (std::size_t i) { return m_value[i]; }

	template<typename Q = T>
	std::enable_if_t<std::is_pointer_v<Q>, const std::remove_pointer_t<T>&>
	operator [] (std::size_t i) const { return m_value[i]; }

	template<typename Q = T>
	std::enable_if_t<std::is_class_v<Q>, typename Q::value_type&>
	operator [] (std::size_t i) { return m_value[i]; }

	template<typename Q = T>
	std::enable_if_t<std::is_class_v<Q>, const typename Q::value_type&>
	operator [] (std::size_t i) const { return m_value[i]; }

	template<typename F, typename... Args>
	auto invoke(F&& f, Args&&... args) -> std::invoke_result_t<F, T, Args...>
	{
		return std::invoke(std::forward<F>(f), m_value, std::forward<Args>(args)...);
	}

	using update_event_proc_t = std::function<void(const property&)>;

	void add_update_event(update_event_proc_t&& proc) const
	{
		m_update_event_proc_list.push_back(std::forward<update_event_proc_t>(proc));
	}

private:
	T m_value = T{};

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		for(auto& proc : m_update_event_proc_list)
			proc(*this);
	}
};

template<typename T>
class property<T[]>
{
public:
	template<typename T2>
	friend class property;

	property(T* v = nullptr) : m_value(v) {}

	property(const property&) = delete;
	property(property&& p) : m_value(p.m_value) { p.m_value = nullptr; }

	template<typename T2>
	property(const property<T2[]>&) = delete;

	template<typename T2>
	property(property<T2[]>&& p) : m_value(p.m_value) { p.m_value = nullptr; }

	property& operator = (const T*&) = delete;

	property& operator = (T*&& v)
	{
		delete [] m_value;
		m_value = v;
		v = nullptr;
		fire_update_event();
		return *this;
	}

	property& operator = (const property&) = delete;

	property& operator = (property&& p)
	{
		delete [] m_value;
		m_value = p.m_value;
		p.m_value = nullptr;
		fire_update_event();
		return *this;
	}

	T& operator [] (std::size_t i) const { return m_value[i]; }

	operator const T* () const { return m_value; }

	using update_event_proc_t = std::function<void(const property&)>;

	void add_update_event(update_event_proc_t&& proc) const
	{ m_update_event_proc_list.push_back(std::forward<update_event_proc_t>(proc)); }

private:
	T* m_value = nullptr;

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		for(auto& proc : m_update_event_proc_list)
			proc(*this);
	}
};

template<typename T, typename... Args>
auto make_property(Args&&... args)
{
	return property<T>{ T { std::forward<Args>(args)... } };
}
