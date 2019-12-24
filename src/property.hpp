#pragma once

#include <vector>
#include <utility>
#include <algorithm>
#include <functional>
#include <type_traits>

template<typename T>
struct default_property_execution_policy
{
	typedef T type;

	const type& execute_copy_out(const type& v) const { return v; }
	type&& execute_move_out(type&& v) { return std::move(v); }
	void execute_copy_in(type& v, const type& nv) { v = nv; }
	void execute_move_in(type& v, type&& nv) { v = std::move(nv); }
};

template<typename T>
struct default_property_execution_policy<T*>
{
	typedef T* type;

	type execute_copy_out(type v) const { return v; }
	type&& execute_move_out(type&& v) { return std::move(v); }
	void execute_move_in(type& v, type&& nv) { delete v; v = nv; nv = nullptr; }
};

template<typename T>
struct default_property_execution_policy<T[]>
{
	typedef T* type;

	type execute_copy_out(type v) const { return v; }
	type&& execute_move_out(type&& v) { return std::move(v); }
	void execute_move_in(type& v, type&& nv) { delete [] v; v = nv; nv = nullptr; }
};

template<typename T, typename P = default_property_execution_policy<T>>
class property : private P
{
public:
	template<typename T2, typename P2>
	friend class property;

	typedef T type;
	typedef T value_type;

	property() = default;

	template<typename... A>
	property(A&&... v)
	: m_value(std::forward<A>(v)...) {}

	property(const property& p)
	: m_value(p.P::execute_copy_out(p.m_value)) {}

	property(property&& p)
	: m_value(p.P::execute_move_out(std::forward<type>(p.m_value))) {}

	template<typename T2, typename P2>
	property(const property<T2, P2>& p)
	: m_value(p.P2::execute_copy_out(p.m_value))
	{ static_assert(std::is_convertible<T2, T>::value); }

	template<typename T2, typename P2>
	property(property<T2, P2>&& p)
	: m_value(p.P2::execute_move_out(std::forward<T2>(p.m_value)))
	{ static_assert(std::is_convertible<T2, T>::value); }

	property& operator = (const type& v)
	{
		P::execute_copy_in(m_value, v);
		fire_update_event();
		return *this;
	}

	property& operator = (type&& v)
	{
		P::execute_move_in(m_value, std::forward<type>(v));
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		P::execute_copy_in(m_value, p.P::execute_copy_out(p.m_value));
		fire_update_event();
		return *this;
	}

	property& operator = (property&& p)
	{
		P::execute_move_in(m_value, p.P::execute_move_out(std::forward<type>(p.m_value)));
		fire_update_event();
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (const property<T2, P2>& p)
 	{
		static_assert(std::is_convertible<T2, T>::value);
		P::execute_copy_in(m_value, p.P2::execute_copy_out(p.m_value));
		fire_update_event();
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (property<T2, P2>&& p)
	{
		static_assert(std::is_convertible<T2, T>::value);
		P::execute_move_in(m_value, p.P2::execute_move_out(std::forward<T2>(p.m_value)));
		fire_update_event();
		return *this;
	}

	operator const type& () const { return P::execute_copy_out(m_value); }

	using update_event_proc_t = std::function<void(const property&)>;

	void operator += (const update_event_proc_t& proc) const
	{ m_update_event_proc_list.push_back(proc); }

	void operator += (update_event_proc_t&& proc) const
	{ m_update_event_proc_list.emplace_back(std::move(proc)); }

private:
	type m_value = type{};

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		std::for_each(
			std::cbegin(m_update_event_proc_list),
			std::cend(m_update_event_proc_list),
			[this](auto& proc) { proc(*this); });
	}
};

template<typename T, typename P>
class property<T*, P> : private P
{
public:
	template<typename T2, typename P2>
	friend class property;

	typedef T* type;
	typedef T value_type;

	property() = default;

	property(type v)
	: m_value(v) {}

	property(const property&) = delete;

	property(property&& p)
	: m_value(p.P::execute_move_out(std::forward<type>(p.m_value)))
	{ p.m_value = nullptr; }

	template<typename T2, typename P2>
	property(const property<T2*, P2>&) = delete;

	template<typename T2, typename P2>
	property(property<T2*, P2>&& p)
	: m_value(p.P2::execute_move_out(std::forward<T2*>(p.m_value)))
	{
		static_assert(std::is_convertible<T2, T>::value);
		p.m_value = nullptr;
	}

	~property() { delete m_value; }

	property& operator = (const type&) = delete;

	property& operator = (type&& v)
	{
		P::execute_move_in(m_value, std::forward<type>(v));
		fire_update_event();
		return *this;
	}

	property& operator = (const property&) = delete;

	property& operator = (property&& p)
	{
		P::execute_move_in(m_value, p.P::execute_move_out(std::forward<type>(p.m_value)));
		fire_update_event();
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (const property<T2*, P2>&) = delete;

	template<typename T2, typename P2>
	property& operator = (property<T2*, P2>&& p)
	{
		static_assert(std::is_convertible<T2, T>::value);
		P::execute_move_in(m_value, p.P2::execute_move_out(std::forward<T2*>(p.m_value)));
		fire_update_event();
		return *this;
	}

	value_type& operator * () const { return *m_value; }

	operator const type () const { return P::execute_copy_out(m_value); }

	using update_event_proc_t = std::function<void(const property&)>;

	void operator += (const update_event_proc_t& proc) const
	{ m_update_event_proc_list.push_back(proc); }

	void operator += (update_event_proc_t&& proc) const
	{ m_update_event_proc_list.emplace_back(std::move(proc)); }

private:
	type m_value = nullptr;

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		std::for_each(
			std::cbegin(m_update_event_proc_list),
			std::cend(m_update_event_proc_list),
			[this](auto& proc) { proc(*this); });
	}
};

template<typename T, typename P>
class property<T[], P> : private P
{
public:
	template<typename T2, typename P2>
	friend class property;

	typedef T* type;
	typedef T value_type;

	property() = default;

	property(type v)
	: m_value(v) {}

	property(const property&) = delete;

	property(property&& p)
	: m_value(p.P::move_out(std::forward<type>(p.m_value)))
	{ p.m_value = nullptr; }

	template<typename T2, typename P2>
	property(const property<T2[], P2>&) = delete;

	template<typename T2, typename P2>
	property(property<T2[], P2>&& p)
	: m_value(p.P2::execute_move_out(std::forward<T2*>(p.m_value)))
	{
		static_assert(std::is_convertible<T2, T>::value);
		p.m_value = nullptr;
	}

	~property() { delete [] m_value; }

	property& operator = (const type&) = delete;

	property& operator = (type&& v)
	{
		P::execute_move_in(m_value, std::forward<type>(v));
		fire_update_event();
		return *this;
	}

	property& operator = (const property&) = delete;

	property& operator = (property&& p)
	{
		P::execute_move_in(m_value, p.P::execute_move_out(std::forward<type>(p.m_value)));
		fire_update_event();
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (const property<T2[], P2>&) = delete;

	template<typename T2, typename P2>
	property& operator = (property<T2[], P2>&& p)
	{
		static_assert(std::is_convertible<T2, T>::value);
		P::execute_move_in(m_value, p.P2::execute_move_out(std::forward<T2*>(p.m_value)));
		fire_update_event();
		return *this;
	}

	value_type& operator [] (std::size_t i) const { return m_value[i]; }

	operator const type () const { return P::execute_copy_out(m_value); }

	using update_event_proc_t = std::function<void(const property&)>;

	void operator += (const update_event_proc_t& proc) const
	{ m_update_event_proc_list.push_back(proc); }

	void operator += (update_event_proc_t&& proc) const
	{ m_update_event_proc_list.emplace_back(std::move(proc)); }

private:
	type m_value = nullptr;

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		std::for_each(
			std::cbegin(m_update_event_proc_list),
			std::cend(m_update_event_proc_list),
			[this](auto& proc) { proc(*this); });
	}
};

template<typename T, typename... A>
auto make_property(A&&... v)
{
	return property<T>{ T { std::forward<A>(v)... } };
}
