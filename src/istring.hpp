#pragma once

#include <istream>
#include <ostream>
#include <compare>
#include <string>
#include <locale>
#include <utility>
#include <algorithm>
#include <type_traits>

inline namespace detail
{
	template<typename CharT>
	inline auto char_ieq(CharT c1, CharT c2, const std::locale& loc = std::locale())
	{
		return std::toupper(c1, loc) == std::toupper(c2, loc);
	};

	template<typename CharT>
	inline auto char_ilt(CharT c1, CharT c2, const std::locale& loc = std::locale())
	{
		return std::toupper(c1, loc) < std::toupper(c2, loc);
	};

	template<typename CharT>
	inline auto string_icmp(const CharT* s1, std::size_t n1, const CharT* s2, std::size_t n2, const std::locale& loc = std::locale())
	{
		if(std::lexicographical_compare(s1, s1 + n1, s2, s2 + n2, [&](CharT c1, CharT c2) { return char_ilt(c1, c2, loc); })) return -1;
		if(std::lexicographical_compare(s2, s2 + n2, s1, s1 + n1, [&](CharT c1, CharT c2) { return char_ilt(c1, c2, loc); })) return 1;
		return 0;
	}

	template<typename CharT>
	struct char_itraits : std::char_traits<CharT>
	{
		static auto eq(CharT c1, CharT c2)
		{
			return char_ieq(c1, c2);
		}

		static auto lt(CharT c1, CharT c2)
		{
			return char_ilt(c1, c2);
		}

		static auto compare(const CharT* s1, const CharT* s2, std::size_t n)
		{
			return string_icmp(s1, n, s2, n);
		}
	};
}

template<typename CharT, typename Alloc = std::allocator<CharT>>
class basic_istring : public std::basic_string<CharT, char_itraits<CharT>, Alloc>
{
public:
	using base = std::basic_string<CharT, char_itraits<CharT>, Alloc>;
	using base::base;

	template<typename Traits2, typename Alloc2,
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, void>* = nullptr>
	explicit basic_istring(const std::basic_string<CharT, Traits2, Alloc2>& str)
	: base(str.data(), str.length()) {}

	explicit operator auto () const
	{
		return std::basic_string<CharT>(this->data(), this->length());
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, bool>
	friend operator == (const basic_istring& lhs, std::basic_string<CharT, Traits2, Alloc2>& rhs)
	{
		return string_icmp(lhs.data(), lhs.length(), rhs.data(), rhs.length()) == 0;
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, std::strong_ordering>
	friend operator <=> (const basic_istring& lhs, std::basic_string<CharT, Traits2, Alloc2>& rhs)
	{
		return string_icmp(lhs.data(), lhs.length(), rhs.data(), rhs.length()) <=> 0;
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, bool>
	friend operator == (std::basic_string<CharT, Traits2, Alloc2>& lhs, const basic_istring& rhs)
	{
		return string_icmp(lhs.data(), lhs.length(), rhs.data(), rhs.length()) == 0;
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, std::strong_ordering>
	friend operator <=> (std::basic_string<CharT, Traits2, Alloc2>& lhs, const basic_istring& rhs)
	{
		return string_icmp(lhs.data(), lhs.length(), rhs.data(), rhs.length()) <=> 0;
	}
};

using istring = basic_istring<char>;
using iwstring = basic_istring<wchar_t>;

inline auto& operator >> (std::istream& is, istring& istr)
{
	std::string temp;
	is >> temp;
	istr = istring(temp.data(), temp.length());
	return is;
}

inline auto& operator >> (std::wistream& wis, iwstring& iwstr)
{
	std::wstring temp;
	wis >> temp;
	iwstr = iwstring(temp.data(), temp.length());
	return wis;
}

inline auto& operator << (std::ostream& os, const istring& istr)
{
	os << istr.c_str();
	return os;
}

inline auto& operator << (std::wostream& wos, const iwstring& iwstr)
{
	wos << iwstr.c_str();
	return wos;
}

inline auto operator ""_is(const char* istr, std::size_t len)
{
	return istring(istr, len);
}

inline auto operator ""_iws(const wchar_t* iwstr, std::size_t len)
{
	return iwstring(iwstr, len);
}
