#pragma once

#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include <iterator>
#include <typeinfo>
#include <unordered_map>
#include <cstddef>
#include <cstdint>



using type_tag_t = std::uint8_t;
using type_tag_ptr_t = type_tag_t*;

template<typename T>
auto get_type_tag()
{
	using type_hash_code_t = decltype(std::declval<std::type_info>().hash_code());
	using type_tag_map_t = std::unordered_map<type_hash_code_t, type_tag_t>;

	static auto k_next_type_tag = type_tag_t(1);
	/*static const auto k_map = type_tag_map_t
	{
		{ typeid(N).hash_code(),           k_next_type_tag++ },
		{ typeid(char).hash_code(),        k_next_type_tag++ },
		{ typeid(const char*).hash_code(), k_next_type_tag++ }, // not needed because of 'const char*' to 'std::string' transform
		{ typeid(short).hash_code(),       k_next_type_tag++ },
		{ typeid(int).hash_code(),         k_next_type_tag++ },
		{ typeid(long).hash_code(),        k_next_type_tag++ },
		{ typeid(long long).hash_code(),   k_next_type_tag++ },
		{ typeid(std::size_t).hash_code(), k_next_type_tag++ },
		{ typeid(std::string).hash_code(), k_next_type_tag++ },
	};
	return k_map.at(typeid(T).hash_code());*/

	static auto k_map = type_tag_map_t();
	auto hash = typeid(T).hash_code();
	auto it = k_map.find(hash);
	if(it == std::end(k_map))
		k_map.insert(std::make_pair(hash, k_next_type_tag++));
	return k_map.at(hash);
}



template<typename T>
constexpr decltype(auto) pack_type_transform(T&& value) { return(value); }

template<typename T>
constexpr decltype(auto) unpack_type_transform(T&& value) { return(value); }

#define MAKE_PACK_TYPE_CAST(from, to) \
	inline auto pack_type_transform(from value) { return to(value); } \
	inline auto unpack_type_transform(to value) { return from(value); }

#define MAKE_PACK_TYPE_TRANSFORM(from, to, pack_proc, unpack_proc) \
	inline auto pack_type_transform(from value) { return pack_proc(value); } \
	inline auto unpack_type_transform(to value) { return unpack_proc(value); }

#define PACK_TYPE_FROM_TYPE(T) std::decay_t<decltype(pack_type_transform(std::declval<T>()))>

MAKE_PACK_TYPE_CAST(std::size_t, short);

MAKE_PACK_TYPE_TRANSFORM(const char*, std::string,
	[](const char* v) { return std::string(v); },
	[](const std::string& v) { return v; });



#define PACK_TYPE_SIZE(T) sizeof(PACK_TYPE_FROM_TYPE(T))

template<typename T>
auto pack_type_size(const T&) { return PACK_TYPE_SIZE(T); }

#define MAKE_PACK_TYPE_SIZE(type, proc) \
	inline auto pack_type_size(type value) { return proc(value); }

MAKE_PACK_TYPE_SIZE(const char*, [](auto v) { return PACK_TYPE_SIZE(decltype(std::strlen(""))) + std::strlen(v); });
MAKE_PACK_TYPE_SIZE(const std::string&, [](auto v) { return PACK_TYPE_SIZE(std::string::size_type) + v.length(); });



using buffer_t = std::vector<std::byte>;
using buffer_ptr_t = buffer_t::const_pointer;
using buffer_iterator_t = buffer_t::const_iterator;

struct buffer_walker_t
{
	buffer_walker_t(buffer_iterator_t& it, std::ptrdiff_t dist)
	: m_it{ it }, m_dist{ dist } {}
	~buffer_walker_t() { m_it += m_dist; }
private:
	buffer_iterator_t& m_it;
	std::ptrdiff_t m_dist;
};



template<typename T>
void pack_bytes(buffer_t& buffer, const T* ptr, std::size_t count)
{
	buffer.insert(std::end(buffer), buffer_ptr_t(ptr), buffer_ptr_t(ptr) + count);
}

template<typename T>
void pack_value(buffer_t& buffer, const T& value)
{
	pack_bytes(buffer, &value, sizeof(value));
}

template<typename T>
void pack_type_tag(buffer_t& buffer)
{
	auto type_tag = get_type_tag<T>();
	pack_value(buffer, type_tag);
}

template<typename T>
void pack_type(buffer_t& buffer, const T& value)
{
	pack_value(buffer, value);
}

inline void pack_type(buffer_t& buffer, const std::string& value)
{
	pack_value<short>(buffer, value.length()); // SSO ;)
	pack_bytes(buffer, value.data(), value.length());
}

template<typename T>
void pack_it(buffer_t& buffer, T&& value)
{
	using PT = PACK_TYPE_FROM_TYPE(T);
	pack_type_tag<PT>(buffer);
	decltype(auto) transformed = pack_type_transform(std::forward<T>(value));
	pack_type(buffer, transformed);
}

template<typename... T>
auto pack(T&&... value)
{
	auto buffer = buffer_t();
	auto value_count = sizeof...(T);
	auto buffer_size = (value_count * sizeof(type_tag_t)) + ((pack_type_size(value)) + ...);
	buffer.reserve(buffer_size);
	(pack_it(buffer, std::forward<T>(value)) , ...);
	return buffer;
}



template<typename T>
decltype(auto) value_cast(buffer_iterator_t& it)
{
	using value_ptr_t = T*;
	return(*value_ptr_t(&*it));
}

template<typename T>
decltype(auto) unpack_value(buffer_iterator_t& it)
{
	decltype(auto) value = value_cast<T>(it);
	auto walk = buffer_walker_t(it, sizeof(value));
	return(value);
}

template<typename T>
void unpack_type_tag(buffer_iterator_t& it)
{
	auto tag1 = get_type_tag<T>();
	auto tag2 = unpack_value<type_tag_t>(it);
	if(tag1 != tag2)
		throw std::bad_typeid();
}

template<typename T>
decltype(auto) unpack_type(buffer_iterator_t& it)
{
	return(unpack_value<T>(it));
}

template<>
decltype(auto) unpack_type<std::string>(buffer_iterator_t& it)
{
	auto length = unpack_value<short>(it); // SSO ;)
	auto walk = buffer_walker_t(it, length);
	return(std::string(std::string::pointer(&*it), length));
}

template<typename T>
decltype(auto)/*auto*/ unpack_it(buffer_iterator_t& it)
{
	using PT = PACK_TYPE_FROM_TYPE(T);
	unpack_type_tag<PT>(it);
	/*decltype(auto) value = unpack_type<PT>(it);
	decltype(auto) transformed = unpack_type_transform(value);
	return transformed;*/
	return(unpack_type_transform(unpack_type<PT>(it)));
}

template<typename... T>
auto unpack(const buffer_t& buffer)
{
	auto it = std::begin(buffer);
	return std::tuple<T...>((unpack_it<T>(it))...);
}
