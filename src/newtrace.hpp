#pragma once

#include <new>
#include <set>
#include <vector>
#include <utility>
#include <iostream>
#include <cstdlib>

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

using ptr_t = void*;
using char_t = char;
using string_t = const char_t*;

struct new_entry_t
{
	new_entry_t(ptr_t p = nullptr, bool a = false, std::size_t b = 0, string_t f = "N/A", string_t pr = "N/A", int l = -1)
	: ptr{ p }, is_array{ a }, bytes{ b }, file{ f }, proc{ pr }, line{ l } {}

	ptr_t ptr;
	bool is_array;
	std::size_t bytes;
	string_t file;
	string_t proc;
	int line;
};

inline bool operator < (const new_entry_t& lhs, const new_entry_t& rhs) { return lhs.ptr < rhs.ptr; }

inline std::ostream& operator << (std::ostream& os, const new_entry_t& entry)
{
	return os;
}

using new_entry_set_t = std::set<new_entry_t, std::less<new_entry_t>, malloc_allocator_t<new_entry_t>>;
using new_entry_list_t = std::vector<new_entry_t, malloc_allocator_t<new_entry_t>>;

inline static auto get_new_entry_set()
{
	static new_entry_set_t* new_entry_set = []()
	{
		void* raw = std::malloc(sizeof(new_entry_set_t));
		if(!raw) throw std::bad_alloc();
		return new (raw) new_entry_set_t;
	}();
	return new_entry_set;
}

inline static auto get_mismatch_list()
{
	static new_entry_list_t* mismatch_list = []()
	{
		void* raw = std::malloc(sizeof(new_entry_list_t));
		if(!raw) throw std::bad_alloc();
		return new (raw) new_entry_list_t;
	}();
	return mismatch_list;
}

inline static void dump_leak()
{
	if(auto leaks = get_new_entry_set(); !leaks->empty())
	{
		std::cerr << "****************************\n";
		std::cerr << "*** MEMORY LEAK(S) FOUND ***\n";
		std::cerr << "****************************\n\n";

		for(const auto& entry : *leaks)
			std::cerr << entry.ptr << " " << entry.bytes << " bytes -> (" << entry.file << " : " << entry.proc << ", line " << entry.line << ", " << (entry.is_array ? "new []" : "new") << ")\n";

		std::cerr << "\n";
	}
}

inline static void dump_mismatch()
{
	if(auto mismatches = get_mismatch_list(); !mismatches->empty())
	{
		std::cerr << "***************************\n";
		std::cerr << "*** NEW/DELETE MISMATCH ***\n";
		std::cerr << "***************************\n\n";

		for(const auto& entry : *mismatches)
			std::cerr << entry.ptr << " " << entry.bytes << " bytes -> (" << entry.file << " : " << entry.proc << ", line " << entry.line << ", " << (entry.is_array ? "new [] & delete" : "new & delete []") << ")\n";

		std::cerr << "\n";
	}
}

inline static void dump_all()
{
	dump_leak();
	dump_mismatch();
}

void __operator_delete__(void* ptr, bool array_delete) noexcept
{
	auto it = get_new_entry_set()->find(ptr);
	if(it != get_new_entry_set()->end())
	{
		if(it->is_array == array_delete)
		{
			get_new_entry_set()->erase(it);
		}
		else
		{
			auto entry = *it;
			get_new_entry_set()->erase(it);
			try { get_mismatch_list()->push_back(std::move(entry)); }
			catch(...) {}
		}
	}
	std::free(ptr);
}

inline static struct __dump_all__ { ~__dump_all__() { dump_all(); } } __dump_all_on_exit__;
} // end anonyous namespace

void* operator new(std::size_t n)
{
	void* ptr = std::malloc(n);
	if(!ptr) throw std::bad_alloc();
	return ptr;
}

void* operator new (std::size_t n, new_entry_t&& entry)
{
	void* ptr = ::operator new(n);
	entry.ptr = ptr;
	try { get_new_entry_set()->insert(std::forward<new_entry_t>(entry)); }
	catch(...) {}
	return ptr;
}

void* operator new (std::size_t n, const char* file, const char* func, int line)
{
	return ::operator new(n, new_entry_t{ nullptr, false, n, file, func, line });
}

void* operator new [] (std::size_t n, const char* file, const char* func, int line)
{
	return ::operator new(n, new_entry_t{ nullptr, true, n, file, func, line });
}

void operator delete (void* ptr) noexcept
{
	__operator_delete__(ptr, false);
}

void operator delete [] (void* ptr) noexcept
{
	__operator_delete__(ptr, true);
}

void operator delete (void* ptr, const char* file, const char* func, int line)
{
	__operator_delete__(ptr, false);
}

void operator delete [] (void* ptr, const char* file, const char* func, int line)
{
	__operator_delete__(ptr, true);
}

#define new new(__FILE_NAME__, __FUNCTION__, __LINE__)
