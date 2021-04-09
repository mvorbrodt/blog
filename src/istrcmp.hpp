#pragma once

#include <istream>
#include <ostream>
#include <iomanip>
#include <compare>
#include <string>
#include <locale>
#include <memory>
#include <utility>
#include <algorithm>
#include <functional>
#include <type_traits>



inline namespace detail
{
	template<typename CharT>
	inline auto char_ieq(CharT c1, CharT c2, const std::locale& loc)
	{
		return std::toupper(c1, loc) == std::toupper(c2, loc);
	};

	template<typename CharT>
	inline auto char_ilt(CharT c1, CharT c2, const std::locale& loc)
	{
		return std::toupper(c1, loc) < std::toupper(c2, loc);
	};
}



/*template<typename CharT, typename T1, typename A1, typename T2, typename A2>
inline auto str_ieq(const std::basic_string<CharT, T1, A1>& s1, const std::basic_string<CharT, T2, A2>& s2, const std::locale& loc = std::locale())
{
	return std::equal(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2),
		[&](CharT c1, CharT c2) { return char_ieq(c1, c2, loc); });
}*/

template<typename CharT, typename T1, typename A1, typename T2, typename A2>
inline auto str_icmp(const std::basic_string<CharT, T1, A1>& s1, const std::basic_string<CharT, T2, A2>& s2, const std::locale& loc = std::locale())
{
	auto r = std::lexicographical_compare(std::begin(s1), std::end(s1), std::begin(s2), std::end(s2),
		[&](CharT c1, CharT c2) { return char_ilt(c1, c2, loc); });
	if(r) return -1;
	r = std::lexicographical_compare(std::begin(s2), std::end(s2), std::begin(s1), std::end(s1),
		[&](CharT c1, CharT c2) { return char_ilt(c1, c2, loc); });
	if(r) return 1;
	return 0;
}



// case insensitive version of std::char_traits...
// ...use with std::basic_string to create case insensitive string class
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
		static const auto loc = std::locale();
		return str_icmp(std::basic_string<CharT>{s1, n}, std::basic_string<CharT>{s2, n}, loc);
	}
};



template<typename CharT, typename Alloc = std::allocator<CharT>>
class basic_istring : public std::basic_string<CharT, char_itraits<CharT>, Alloc>
{
public:
	using base = typename std::basic_string<CharT, char_itraits<CharT>, Alloc>;
	using base::base;

	template<typename Traits2, typename Alloc2,
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, void>* = nullptr>
	basic_istring(const std::basic_string<CharT, Traits2, Alloc2>& str)
	: base(str.data(), str.length()) {}

	operator auto () const
	{
		return std::basic_string<CharT>(this->data(), this->length());
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, bool>
	friend operator == (const basic_istring& lhs, std::basic_string<CharT, Traits2, Alloc2>& rhs)
	{
		return str_icmp(lhs, rhs) == 0;
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, std::strong_ordering>
	friend operator <=> (const basic_istring& lhs, std::basic_string<CharT, Traits2, Alloc2>& rhs)
	{
		return str_icmp(lhs, rhs) <=> 0;
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, bool>
	friend operator == (std::basic_string<CharT, Traits2, Alloc2>& lhs, const basic_istring& rhs)
	{
		return str_icmp(lhs, rhs) == 0;
	}

	template<typename Traits2, typename Alloc2>
	std::enable_if_t<not std::is_same_v<char_itraits<CharT>, Traits2>, std::strong_ordering>
	friend operator <=> (std::basic_string<CharT, Traits2, Alloc2>& lhs, const basic_istring& rhs)
	{
		return str_icmp(lhs, rhs) <=> 0;
	}
};



using istring = basic_istring<char>;
using iwstring = basic_istring<wchar_t>;



inline auto operator ""_is(const char* istr, std::size_t len)
{
	return istring(istr, len);
}

inline auto operator ""_iws(const wchar_t* iwstr, std::size_t len)
{
	return iwstring(iwstr, len);
}

inline auto& operator >> (std::istream& is, istring& istr)
{
	std::string temp;
	is >> temp;
	istr = std::move(temp);
	return is;
}

inline auto& operator >> (std::wistream& wis, iwstring& iwstr)
{
	std::wstring temp;
	wis >> temp;
	iwstr = std::move(temp);
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
