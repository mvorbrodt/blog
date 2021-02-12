#pragma once

#include <new>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <cstdlib>
#define BOOST_STACKTRACE_GNU_SOURCE_NOT_REQUIRED
#include <boost/stacktrace.hpp>

namespace
{
	template<typename T>
	struct malloc_allocator_t : std::allocator<T>
	{
		T* allocate(std::size_t n)
		{
			T* ptr = (T*)std::malloc(n * sizeof(T));
			if(!ptr) throw std::bad_alloc();
			return ptr;
		}

		void deallocate(T* ptr, std::size_t) { std::free(ptr); }

		template<typename U>
		struct rebind { typedef malloc_allocator_t<U> other; };
	};

	using char_t = char;
	using string_t = const char_t*;
	using stack_trace_t = std::string;
	using new_entry_t = struct { std::size_t bytes; string_t file; string_t proc; int line; stack_trace_t stack; };
	using ptr_t = void*;
	using new_ptr_map_t = std::map<ptr_t, new_entry_t, std::less<ptr_t>, malloc_allocator_t<std::pair<ptr_t const, new_entry_t>>>;
	using leak_list_t = std::vector<new_entry_t>;

	inline static auto& get_ptr_map()
	{
		static new_ptr_map_t new_ptr_map;
		return new_ptr_map;
	}

	inline static auto get_leaks()
	{
		leak_list_t leaks;
		leaks.reserve(get_ptr_map().size());
		for(const auto& it : get_ptr_map())
			leaks.push_back(it.second);
		return leaks;
	}
}

void* operator new(std::size_t n)
{
	void* ptr = std::malloc(n);
	if(!ptr) throw std::bad_alloc();
	return ptr;
}

void* operator new (std::size_t n, const char* file, const char* func, int line)
{
	void* ptr = ::operator new(n);
	try {
		namespace st = boost::stacktrace;
		auto stack = st::to_string(st::stacktrace());
		get_ptr_map().emplace(ptr, new_entry_t{ n, file, func, line, std::move(stack) });
	} catch(...) { }
	return ptr;
}

void* operator new [] (std::size_t n, const char* file, const char* func, int line)
{
	return ::operator new(n, file, func, line);
}

void operator delete (void* ptr) noexcept
{
	auto it = get_ptr_map().find(ptr);
	if(it != get_ptr_map().end())
		get_ptr_map().erase(it);
	std::free(ptr);
}

#define new new(__FILE_NAME__, __FUNCTION__, __LINE__)
