#pragma once

#include <iostream>
#include <ostream>
#include <vector>
#include <chrono>
#include <type_traits>
#include <initializer_list>
#include <cstddef>



template<typename T, typename... Args>
inline auto make_vector(std::size_t N, Args&&... args)
{
	std::vector<T> result;
	result.reserve(N);

	while(N--)
		result.emplace_back(std::forward<Args>(args)...);

	return result;
}

template<typename T, typename U>
inline auto make_vector(std::size_t N, std::initializer_list<U> l)
{
	std::vector<T> result;
	result.reserve(N);

	while(N--)
		result.emplace_back(l);

	return result;
}



template<typename ResT, typename RepT, typename CharT>
struct basic_stack_timer
{
	using StrT = std::add_pointer_t<std::add_const_t<CharT>>;

	explicit basic_stack_timer(StrT prefix = StrT("duration: "), StrT postfix = StrT(" s"), std::basic_ostream<CharT>& output = std::cout)
	: m_prefix{ prefix }, m_postfix{ postfix }, m_output{ output } {}

	~basic_stack_timer()
	{
		auto end_time = std::chrono::high_resolution_clock::now();

		using duration_t = std::chrono::duration<RepT, typename ResT::period>;
		auto duration = std::chrono::duration_cast<duration_t>(end_time - m_start_time);

		m_output << m_prefix << duration.count() << m_postfix << std::endl;
	}

	void* operator new (std::size_t) = delete;
	void* operator new [] (std::size_t) = delete;

private:
	StrT m_prefix;
	StrT m_postfix;
	std::basic_ostream<CharT>& m_output;
	std::chrono::high_resolution_clock::time_point m_start_time = std::chrono::high_resolution_clock::now();
};

using stack_timer = basic_stack_timer<std::chrono::seconds, float, char>;
//using stack_timer = basic_stack_timer<std::chrono::milliseconds, int, wchar_t>;

