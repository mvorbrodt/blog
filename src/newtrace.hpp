#pragma once

#if not defined(__cplusplus) or __cplusplus < 201703L
#error C++17 compiler is required!"
#endif

#include <new>
#include <mutex>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_set>
#include <iostream>
#include <ostream>
#include <cstdlib>

namespace ndt::detail
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
	};

	struct new_entry_t
	{
		new_entry_t(void* p = nullptr, bool a = false, std::size_t b = 0,
			const char* f = "N/A", int l = -1, const char* fn = "N/A")
		: ptr{ p }, is_array{ a }, bytes{ b }, file{ f }, line{ l }, func{ fn } {}

		void*       ptr;
		bool        is_array;
		std::size_t bytes;
		const char* file;
		int         line;
		const char* func;
	};

	inline std::ostream& operator << (std::ostream& os, const new_entry_t& entry)
	{
		os << entry.bytes << "B leaked using '" << (entry.is_array ? "new[]" : "new")
			<< "' -> '" << entry.file << ":" << entry.line << "' in '" << entry.func << "'";
		return os;
	}

	inline bool operator == (const new_entry_t& lhs, const new_entry_t& rhs) { return lhs.ptr == rhs.ptr; }

	struct new_entry_hash_t : std::hash<void*>
	{
		using base = std::hash<void*>;
		std::size_t operator()(const new_entry_t& entry) const { return base::operator()(entry.ptr); }
	};

	using new_entry_set_t = std::unordered_set<new_entry_t, new_entry_hash_t, std::equal_to<new_entry_t>, malloc_allocator_t<new_entry_t>>;

	using new_mismatch_list_t = std::vector<new_entry_t, malloc_allocator_t<new_entry_t>>;

	inline auto get_new_entry_set()
	{
		static new_entry_set_t* new_entry_set = []()
		{
			void* raw = std::malloc(sizeof(new_entry_set_t));
			if(!raw) throw std::bad_alloc();
			return new (raw) new_entry_set_t;
		}();
		return new_entry_set;
	}

	inline auto get_new_mismatch_list()
	{
		static new_mismatch_list_t* new_mismatch_list = []()
		{
			void* raw = std::malloc(sizeof(new_mismatch_list_t));
			if(!raw) throw std::bad_alloc();
			return new (raw) new_mismatch_list_t;
		}();
		return new_mismatch_list;
	}

	inline void operator_delete(void* ptr, bool array_delete) noexcept
	{
		static std::recursive_mutex operator_delete_lock;
		std::scoped_lock guard{ operator_delete_lock };
		auto it = get_new_entry_set()->find(ptr);
		if(it != get_new_entry_set()->end())
		{
			if(it->is_array == array_delete)
			{
				get_new_entry_set()->erase(it);
			}
			else
			{
				try { get_new_mismatch_list()->push_back(*it); }
				catch(...) {}
				get_new_entry_set()->erase(it);
			}
		}
		std::free(ptr);
	}
}

namespace ndt
{
	inline void dump_leak()
	{
		if(auto leaks = detail::get_new_entry_set(); !leaks->empty())
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
		if(auto mismatches = detail::get_new_mismatch_list(); !mismatches->empty())
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
}

#ifdef ENABLE_NEW_DELETE_TRACE_DUMP
namespace { inline const struct __dump_all__ { ~__dump_all__() { ndt::dump_all(); } } __dump_all_on_exit__; }
#endif

void* operator new (std::size_t n)
{
	void* ptr = std::malloc(n);
	if(!ptr) throw std::bad_alloc();
	return ptr;
}

void* operator new (std::size_t n, ndt::detail::new_entry_t entry)
{
	void* ptr = ::operator new(n);
	entry.ptr = ptr;
	try
	{
		static std::recursive_mutex operator_new_lock;
		std::scoped_lock guard{ operator_new_lock };
		ndt::detail::get_new_entry_set()->insert(entry);
	}
	catch(...) {}
	return ptr;
}

void* operator new (std::size_t n, const char* file, int line, const char* func)
{
	return ::operator new(n, ndt::detail::new_entry_t{ nullptr, false, n, file, line, func });
}

void* operator new [] (std::size_t n, const char* file, int line, const char* func)
{
	return ::operator new(n, ndt::detail::new_entry_t{ nullptr, true, n, file, line, func });
}

void operator delete (void* ptr) noexcept
{
	ndt::detail::operator_delete(ptr, false);
}

void operator delete [] (void* ptr) noexcept
{
	ndt::detail::operator_delete(ptr, true);
}

void operator delete (void* ptr, const char*, int, const char*) noexcept
{
	ndt::detail::operator_delete(ptr, false);
}

void operator delete [] (void* ptr, const char*, int, const char*) noexcept
{
	ndt::detail::operator_delete(ptr, true);
}

#warning If '__PRETTY_FUNCTION__' is undefined replace it with '__proc__' below. \
Otherwise comment these warnings out and hope we get 'std::source_location' soon! \
https://en.cppreference.com/w/cpp/utility/source_location/

#if defined(new)
#error Macro 'new' is already defined!
#else
#define new new(__FILE__, __LINE__, __PRETTY_FUNCTION__)
#endif
