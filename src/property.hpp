#pragma once

#include <vector>
#include <utility>
#include <functional>

template<typename T>
class property
{
public:
	template<typename T2>
	friend class property;

	property() = default;

	property(const T& v) : m_value(v) {}
	property(T&& v) : m_value(std::move(v)) {}

	property(const property& p) : m_value(p.m_value) {}
	property(property&& p) : m_value(std::move(p.m_value)) {}

	template<typename T2>
	property(const property<T2>& p) : m_value(p.m_value) {}

	template<typename T2>
	property(property<T2>&& p) : m_value(std::move(p.m_value)) {}

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
		m_value = p.m_value;
		fire_update_event();
		return *this;
	}

	property& operator = (property&& p)
	{
		m_value = std::move(p.m_value);
		fire_update_event();
		return *this;
	}

	template<typename T2>
	property& operator = (const property<T2>& p)
 	{
		m_value = p.m_value;
		fire_update_event();
		return *this;
	}

	template<typename T2>
	property& operator = (property<T2>&& p)
	{
		m_value = std::move(p.m_value);
		fire_update_event();
		return *this;
	}



	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator ++ ()
	{
		++m_value;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator ++ (int)
	{
		++m_value;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator += (const Q& nv)
	{
		m_value += nv;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator -= (const Q& nv)
	{
		m_value -= nv;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator *= (const Q& nv)
	{
		m_value *= nv;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator /= (const Q& nv)
	{
		m_value /= nv;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator += (const property& p)
	{
		m_value += p.m_value;;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator -= (const property& p)
	{
		m_value -= p.m_value;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator *= (const property& p)
	{
		m_value *= p.m_value;;
		fire_update_event();
		return *this;
	}

	template<typename Q = T>
	typename std::enable_if<std::is_arithmetic_v<Q>, property&>::type
	operator /= (const property& p)
	{
		m_value /= p.m_value;
		fire_update_event();
		return *this;
	}



	operator const T& () const { return m_value; }

	using update_event_proc_t = std::function<void(const property&)>;

	void operator += (update_event_proc_t&& proc) const
	{ m_update_event_proc_list.push_back(std::forward<update_event_proc_t>(proc)); }

private:
	T m_value = T{};

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		for(auto& proc : m_update_event_proc_list)
			{ try { proc(*this); } catch(...) {} };
	}
};

template<typename T>
class property<T*>
{
public:
	template<typename T2>
	friend class property;

	property(T* v = nullptr) : m_value(v) {}

	property(const property&) = delete;
	property(property&& p) : m_value(p.m_value) { p.m_value = nullptr; }

	template<typename T2>
	property(const property<T2*>&) = delete;

	template<typename T2>
	property(property<T2*>&& p) : m_value(p.m_value) { p.m_value = nullptr; }

	~property() { delete m_value; }

	property& operator = (const T*&) = delete;

	property& operator = (T*&& v)
	{
		delete m_value;
		m_value = v;
		v = nullptr;
		fire_update_event();
		return *this;
	}

	property& operator = (const property&) = delete;

	property& operator = (property&& p)
	{
		delete m_value;
		m_value = p.m_value;
		p.m_value = nullptr;
		fire_update_event();
		return *this;
	}

	template<typename T2>
	property& operator = (const property<T2*>&) = delete;

	template<typename T2>
	property& operator = (property<T2*>&& p)
	{
		delete m_value;
		m_value = p.m_value;
		p.m_value = nullptr;
		fire_update_event();
		return *this;
	}

	T& operator * () const { return *m_value; }

	operator const T* () const { return m_value; }

	using update_event_proc_t = std::function<void(const property&)>;

	void operator += (update_event_proc_t&& proc) const
	{ m_update_event_proc_list.push_back(std::forward<update_event_proc_t>(proc)); }

private:
	T* m_value = nullptr;

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		for(auto& proc : m_update_event_proc_list)
			{ try { proc(*this); } catch(...) {} };
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

	~property() { delete [] m_value; }

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

	template<typename T2>
	property& operator = (const property<T2[]>&) = delete;

	template<typename T2>
	property& operator = (property<T2[]>&& p)
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

	void operator += (update_event_proc_t&& proc) const
	{ m_update_event_proc_list.push_back(std::forward<update_event_proc_t>(proc)); }

private:
	T* m_value = nullptr;

	using update_event_proc_list_t = std::vector<update_event_proc_t>;
	mutable update_event_proc_list_t m_update_event_proc_list;

	void fire_update_event() const
	{
		for(auto& proc : m_update_event_proc_list)
			{ try { proc(*this); } catch(...) {} };
	}
};

template<typename T, typename... A>
auto make_property(A&&... v)
{
	return property<T>{ T { std::forward<A>(v)... } };
}
