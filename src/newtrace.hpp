#pragma once

#ifndef __cplusplus
#error C++ compiler is required!"
#endif

#if __cplusplus < 201703L
#error C++17 compiler is required!
#endif

static_assert(__cplusplus >= 201703L, "C++17 compiler is required!");

#ifdef new
#undef new
#endif

#include <new>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
#include <iostream>
#include <ostream>
#include <cstdlib>

namespace __new_delete_trace_impl__
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
		new_entry_t(ptr_t p = nullptr, bool a = false, std::size_t b = 0,
			string_t f = "N/A", int l = -1, string_t fn = "N/A")
		: ptr{ p }, is_array{ a }, bytes{ b }, file{ f }, line{ l }, func{ fn } {}

		ptr_t       ptr;
		bool        is_array;
		std::size_t bytes;
		string_t    file;
		int         line;
		string_t    func;
	};

	inline std::ostream& operator << (std::ostream& os, const new_entry_t& entry)
	{
		os << entry.bytes << "B leaked using '" << (entry.is_array ? "new[]" : "new")
			<< "' -> '" << entry.file << ":" << entry.line << "' in '" << entry.func << "'";
		return os;
	}

	inline bool operator == (const new_entry_t& lhs, const new_entry_t& rhs) { return lhs.ptr == rhs.ptr; }

	struct new_entry_hash_t : std::hash<ptr_t>
	{
		using base = std::hash<ptr_t>;
		std::size_t operator()(const new_entry_t& entry) { return base::operator()(entry.ptr); }
	};

	using new_entry_set_t = std::unordered_set<new_entry_t, new_entry_hash_t, std::equal_to<new_entry_t>, malloc_allocator_t<new_entry_t>>;

	using new_entry_list_t = std::vector<new_entry_t, malloc_allocator_t<new_entry_t>>;

	inline auto get_new_entry_set()
	{
		static new_entry_set_t* new_entry_set = []()
		{
			ptr_t raw = std::malloc(sizeof(new_entry_set_t));
			if(!raw) throw std::bad_alloc();
			return new (raw) new_entry_set_t;
		}();
		return new_entry_set;
	}

	inline auto get_mismatch_list()
	{
		static new_entry_list_t* mismatch_list = []()
		{
			ptr_t raw = std::malloc(sizeof(new_entry_list_t));
			if(!raw) throw std::bad_alloc();
			return new (raw) new_entry_list_t;
		}();
		return mismatch_list;
	}

	inline void dump_leak()
	{
		if(auto leaks = get_new_entry_set(); !leaks->empty())
		{
			std::cerr << "****************************\n";
			std::cerr << "*** MEMORY LEAK(S) FOUND ***\n";
			std::cerr << "****************************\n\n";

			for(auto& entry : *leaks)
				std::cerr << entry << "\n";

			std::cerr << "\n";
		}
	}

	inline void dump_mismatch()
	{
		if(auto mismatches = get_mismatch_list(); !mismatches->empty())
		{
			std::cerr << "***************************\n";
			std::cerr << "*** NEW/DELETE MISMATCH ***\n";
			std::cerr << "***************************\n\n";

			for(auto& entry : *mismatches)
				std::cerr << entry << ", freed using '" << (entry.is_array ? "delete" : "delete[]") << "'\n";

			std::cerr << "\n";
		}
	}

	inline void dump_all()
	{
		dump_leak();
		dump_mismatch();
	}

	inline void operator_delete(ptr_t ptr, bool array_delete) noexcept
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
				try { get_mismatch_list()->push_back(*it); }
				catch(...) {} // Eat all exceptions, we don't want to get in the way...
				get_new_entry_set()->erase(it);
			}
		}
		std::free(ptr);
	}
} // End of __new_delete_trace_impl__ namespace

namespace ndt = ::__new_delete_trace_impl__;

#ifdef NEW_DELETE_TRACE_DUMP
// If NEW_DELETE_TRACE_DUMP is #define'd, this line will dump leaks/mismatches on program exit
namespace { inline const struct __dump_all__ { ~__dump_all__() { ndt::dump_all(); } } __dump_all_on_exit__; }
#endif

ndt::ptr_t operator new(std::size_t n)
{
	ndt::ptr_t ptr = std::malloc(n);
	if(!ptr) throw std::bad_alloc();
	return ptr;
}

ndt::ptr_t operator new (std::size_t n, ndt::new_entry_t entry)
{
	ndt::ptr_t ptr = ::operator new(n);
	entry.ptr = ptr;
	try { ndt::get_new_entry_set()->insert(entry); }
	catch(...) {} // Eat all exceptions, we don't want to get in the way...
	return ptr;
}

ndt::ptr_t operator new (std::size_t n, ndt::string_t file, int line, ndt::string_t func)
{
	return ::operator new(n, ndt::new_entry_t{ nullptr, false, n, file, line, func });
}

ndt::ptr_t operator new [] (std::size_t n, ndt::string_t file, int line, ndt::string_t func)
{
	return ::operator new(n, ndt::new_entry_t{ nullptr, true, n, file, line, func });
}

void operator delete (ndt::ptr_t ptr) noexcept
{
	ndt::operator_delete(ptr, false);
}

void operator delete [] (ndt::ptr_t ptr) noexcept
{
	ndt::operator_delete(ptr, true);
}

void operator delete (ndt::ptr_t ptr, ndt::string_t, int, ndt::string_t) noexcept
{
	ndt::operator_delete(ptr, false);
}

void operator delete [] (ndt::ptr_t ptr, ndt::string_t, int, ndt::string_t) noexcept
{
	ndt::operator_delete(ptr, true);
}

#warning If '__PRETTY_FUNCTION__' is undefined replace it with '__proc__' below. \
Otherwise comment these warnings out and hope we get 'std::source_location' soon! \
https://en.cppreference.com/w/cpp/utility/source_location/

#ifndef new
#define new new(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
