#pragma once

#include <utility>
#include <type_traits>

template<typename T, typename AP, typename RP>
class basic_handle : public AP, public RP
{
public:
	basic_handle(const T& v) : m_handle(v) { AP::Execute(m_handle); }

	template<typename U = T, std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
	basic_handle(U&& v) : m_handle(std::move(v)) { AP::Execute(m_handle); v = U{}; }

	basic_handle(const basic_handle&) = delete;

	template<typename U = T, std::enable_if_t<std::is_default_constructible_v<U>>* = nullptr>
	basic_handle(basic_handle&& other) : m_handle(std::move(other.m_handle)) { other.m_handle = U{}; }

	~basic_handle() { RP::Execute(m_handle); }

	basic_handle& operator = (const T& v) = delete;

	template<typename U = T>
	std::enable_if_t<std::is_default_constructible_v<U>, basic_handle&>
	operator = (U&& v)
	{
		RP::Execute(m_handle);
		m_handle = std::move(v);
		AP::Execute(m_handle);
		v = U{};
		return *this;
	}

	basic_handle& operator = (const basic_handle&) = delete;

	template<typename U = T>
	std::enable_if_t<std::is_default_constructible_v<U>, basic_handle&>
	operator = (basic_handle&& other)
	{
		RP::Execute(m_handle);
		m_handle = std::move(other.m_handle);
		other.m_handle = U{};
		return *this;
	}

	operator T& () { return m_handle; }
	operator const T& () const { return m_handle; }

	template<typename U = T>
	std::enable_if_t<!std::is_reference_v<U>, U*>
	operator & () { return &m_handle; }

private:
	T m_handle;
};

struct NoOpPolicy { template<typename T> void Execute(const T&) {} };

struct PointerReleasePolicy { template<typename T> void Execute(T* ptr) { delete ptr; } };
template<typename T> using ptr_handle_t = basic_handle<T*, NoOpPolicy, PointerReleasePolicy>;

struct PointerArrayReleasePolicy { template<typename T> void Execute(T* ptr) { delete[] ptr; } };
template<typename T> using arr_ptr_handle_t = basic_handle<T*, NoOpPolicy, PointerArrayReleasePolicy>;
