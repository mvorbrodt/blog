#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>

template<typename T>
struct default_property_policy
{
	const T& copy_out(const T& value) const { std::cout << "!!! read_out" << std::endl; return value; }
	T&& move_out(T&& value) { std::cout << "!!! move_out" << std::endl; return std::move(value); }
	template<typename U> void copy_in(T& value, const U& new_value) { std::cout << "!!! copy_in" << std::endl; value = new_value; }
	template<typename U> void move_in(T& value, U&& new_value) { std::cout << "!!! move_in" << std::endl; value = std::move(new_value); }
};

template<typename T, typename P = default_property_policy<T>>
class property : private P
{
public:
	template<typename T2, typename P2>
	friend class property;

	property() = default;

	property(const T& v) : m_value(v) { std::cout << "copy construct value" << std::endl; }
	property(T&& v) : m_value(std::move(v)) { std::cout << "move construct value" << std::endl; }

	property(const property& p) : m_value(p.P::copy_out(p.m_value)) { std::cout << "copy construct property" << std::endl; }
	property(property&& p) : m_value(std::move(p.P::move_out(std::forward<T>(p.m_value)))) { std::cout << "move construct property" << std::endl; }

	template<typename T2, typename P2>
	property(const property<T2, P2>& p) : m_value(p.P2::copy_out(p.m_value)) { std::cout << "copy construct diff property" << std::endl; }

	template<typename T2, typename P2>
	property(property<T2, P2>&& p) : m_value(std::move(p.P2::move_out(std::forward<T2>(p.m_value)))) { std::cout << "move construct diff property" << std::endl; }

	property& operator = (const T& v)
	{
		std::cout << "copy value" << std::endl;
		P::copy_in(m_value, v);
		fire_update_event();
		return *this;
	}

	property& operator = (T&& v)
	{
		std::cout << "move value" << std::endl;
		P::move_in(m_value, v);
		fire_update_event();
		return *this;
	}

	property& operator = (const property& p)
	{
		std::cout << "copy property" << std::endl;
		P::copy_in(m_value, p.P::copy_out(p.m_value));
		fire_update_event();
		return *this;
	}

	property& operator = (property&& p)
	{
		std::cout << "move property" << std::endl;
		P::move_in(m_value, p.P::move_out(std::forward<T>(p.m_value)));
		fire_update_event();
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (const property<T2, P2>& p)
 	{
		std::cout << "copy diff property" << std::endl;
		P::copy_in(m_value, p.P2::copy_out(p.m_value));
		fire_update_event();
		return *this;
	}

	template<typename T2, typename P2>
	property& operator = (property<T2, P2>&& p)
	{
		std::cout << "move diff property" << std::endl;
		P::move_in(m_value, p.P2::move_out(std::forward<T2>(p.m_value)));
		fire_update_event();
		return *this;
	}

	operator auto () const { return P::copy_out(m_value); }

	using update_event_proc_t = std::function<void(const T&)>;
	void operator += (const update_event_proc_t& proc) const { m_update_event_proc_list.push_back(proc); }
	void operator += (update_event_proc_t&& proc) const { m_update_event_proc_list.emplace_back(std::move(proc)); }

private:
	T m_value = T{};

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		std::for_each(std::cbegin(m_update_event_proc_list), std::cend(m_update_event_proc_list),
			[this](auto& proc) { proc(m_value); });
	}
};
