#pragma once

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <functional>

template<typename T>
struct default_property_read_policy
{
	const T& read(const T& value) const { std::cout << "!!! RP const" << std::endl; return value; }
};

template<typename T>
struct default_property_move_policy
{
	T&& move(T&& value) { std::cout << "!!! MP move" << std::endl; return std::move(value); }
};

template<typename T>
struct default_property_write_policy
{
	template<typename U> void write(T& value, const U& new_value) { std::cout << "!!! WP copy" << std::endl; value = new_value; }
	template<typename U> void write(T& value, U&& new_value) { std::cout << "!!! WP move" << std::endl; value = std::move(new_value); }
};

template
<
	typename T,
	typename RP = default_property_read_policy<T>,
	typename MP = default_property_move_policy<T>,
	typename WP = default_property_write_policy<T>
>
class property : private RP, private MP, private WP
{
public:
	template<typename T2, typename RP2, typename MP2, typename WP2>
	friend class property;

	property() = default;

	property(const T& v) : m_value(v) { std::cout << "copy construct value" << std::endl; }
	property(T&& v) : m_value(std::move(v)) { std::cout << "move construct value" << std::endl; }

	property(const property& p) : m_value(p.RP::read(p.m_value)) { std::cout << "copy construct property" << std::endl; }
	property(property&& p) : m_value(std::move(p.MP::move(std::forward<T>(p.m_value)))) { std::cout << "move construct property" << std::endl; }

	template<typename T2, typename RP2, typename MP2, typename WP2>
	property(const property<T2, RP2, MP2, WP2>& p) : m_value(p.RP2::read(p.m_value)) { std::cout << "copy construct diff property" << std::endl; }

	template<typename T2, typename RP2, typename MP2, typename WP2>
	property(property<T2, RP2, MP2, WP2>&& p) : m_value(std::move(p.MP2::move(std::forward<T2>(p.m_value)))) { std::cout << "move construct diff property" << std::endl; }

	property& operator = (const T& v)
	{
		std::cout << "copy value" << std::endl;
		WP::write(m_value, v);
		fire_update_event_proc();
		return *this;
	}

	property& operator = (T&& v)
	{
		std::cout << "move value" << std::endl;
		WP::write(m_value, v);
		fire_update_event_proc();
		return *this;
	}

	property& operator = (const property& p)
	{
		std::cout << "copy property" << std::endl;
		WP::write(m_value, p.RP::read(p.m_value));
		fire_update_event_proc();
		return *this;
	}

	property& operator = (property&& p)
	{
		std::cout << "move property" << std::endl;
		WP::write(m_value, p.MP::move(std::forward<T>(p.m_value)));
		fire_update_event_proc();
		return *this;
	}

	template<typename T2, typename RP2, typename MP2, typename WP2>
	property& operator = (const property<T2, RP2, MP2, WP2>& p)
 	{
		std::cout << "copy diff property" << std::endl;
		WP::write(m_value, p.RP2::read(p.m_value));
		fire_update_event_proc();
		return *this;
	}

	template<typename T2, typename RP2, typename MP2, typename WP2>
	property& operator = (property<T2, RP2, MP2, WP2>&& p)
	{
		std::cout << "move diff property" << std::endl;
		WP::write(m_value, p.MP2::move(std::forward<T2>(p.m_value)));
		fire_update_event_proc();
		return *this;
	}

	operator auto () const { return RP::read(m_value); }

	using update_event_proc_t = std::function<void(const T&)>;
	void operator += (const update_event_proc_t& proc) const { m_update_event_proc_list.push_back(proc); }
	void operator += (update_event_proc_t&& proc) const { m_update_event_proc_list.emplace_back(std::move(proc)); }

private:
	T m_value = T{};

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event_proc() const
	{
		std::for_each(std::cbegin(m_update_event_proc_list), std::cend(m_update_event_proc_list),
			[this](auto& proc) { proc(m_value); });
	}
};
