#include <iostream>
#include <tuple>
#include <string>
#include <vector>
#include <utility>
#include <iterator>
#include <typeinfo>
#include <algorithm>
#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <cstring>

// noisy test type, to see if we're not creating unnececary instances...
struct N
{
	N(int v) : m_v{ v } { std::cout << "N(" << v << ")\n"; }
	N(const N& o) : m_v{ o.m_v } { std::cout << "N(const N&) " << m_v << "\n"; }
	N(N&& o) : m_v{ o.m_v } { std::cout << "N(NN&) " << m_v << "\n"; }
	~N() { std::cout << "~N(" << m_v << ")\n"; }
	N& operator = (const N& o) { std::cout << "operator=(const N&) " << m_v << ", " << o.m_v << "\n"; return *this; }
	N& operator = (N&& o) { std::cout << "operator=(N&&) " << m_v << ", " << o.m_v << "\n"; return *this; }

private:
	int m_v;
};



enum class type_tag_t : std::uint8_t
{
	N_tag = 1,
	char_tag,
	short_tag,
	int_tag,
	long_tag,
	long_long_tag,
	size_t_tag,
	string_tag,
};

using type_tag_ptr_t = type_tag_t*;

template<typename T>
auto get_type_tag()
{
	using type_hash_code_t = decltype(std::declval<std::type_info>().hash_code());
	using type_tag_map_t = std::unordered_map<type_hash_code_t, type_tag_t>;

	static const auto k_map = type_tag_map_t
	{
		{ typeid(N).hash_code(), type_tag_t::N_tag },
		{ typeid(char).hash_code(), type_tag_t::char_tag },
		{ typeid(const char*).hash_code(), type_tag_t::string_tag }, // store 'const char*' strings, and extract, as std::string
		{ typeid(short).hash_code(), type_tag_t::short_tag },
		{ typeid(int).hash_code(), type_tag_t::int_tag } ,
		{ typeid(long).hash_code(), type_tag_t::long_tag } ,
		{ typeid(long long).hash_code(), type_tag_t::long_long_tag } ,
		//{ typeid(std::size_t).hash_code(), type_tag_t::size_t_tag } , // not needed because std::size_t is stored as short
		{ typeid(std::string).hash_code(), type_tag_t::string_tag } ,
	};

	return k_map.at(typeid(T).hash_code());
}



template<typename T>
constexpr decltype(auto) transform_value_pack(const T& value) { return(value); }

template<typename T>
constexpr decltype(auto) transform_value_unpack(const T& value) { return(value); }

#define PACK_VALUE_TRANSFORM(from, to, proc) \
	inline constexpr to transform_value_pack(from value) { return proc(value); } \
	inline constexpr from transform_value_unpack(to value) { return proc(value); }

PACK_VALUE_TRANSFORM(std::size_t, short, [](auto v) { return v; });



template<typename T>
auto packed_size(const T&) { return sizeof(type_tag_t) + sizeof(transform_value_pack(std::declval<T>())); }

#define PACK_VALUE_SIZE(type, proc) \
	inline auto packed_size(type value) { return proc(value); }

PACK_VALUE_SIZE(const char*, [](auto v) { return sizeof(type_tag_t) + sizeof(transform_value_pack(std::declval<decltype(std::strlen(""))>())) + std::strlen(v); });
PACK_VALUE_SIZE(const std::string&, [](auto v) { return sizeof(type_tag_t) + sizeof(transform_value_pack(std::declval<std::string::size_type>())) + v.length(); });



using buffer_t = std::vector<std::byte>;
using buffer_ptr_t = buffer_t::const_pointer;
using buffer_iterator_t = buffer_t::const_iterator;



template<typename T>
void pack_bytes(buffer_t& buffer, const T* ptr, std::size_t count)
{
	auto bytes = buffer_ptr_t(ptr);
	buffer.insert(std::end(buffer), bytes, bytes + count);
}

template<typename T>
void pack_type_tag(buffer_t& buffer)
{
	auto type_tag = get_type_tag<T>();
	pack_bytes(buffer, &type_tag, sizeof(type_tag));
}

template<typename T>
void pack_value(buffer_t& buffer, const T& value)
{
	decltype(auto) transformed = transform_value_pack(value);
	pack_bytes(buffer, &transformed, sizeof(transformed));
}

template<typename T>
void pack_type(buffer_t& buffer, const T& value)
{
	using value_t = decltype(transform_value_pack(value));
	pack_type_tag<value_t>(buffer);
	pack_value(buffer, value);
}

inline void pack_type(buffer_t& buffer, const char* value)
{
	pack_type_tag<const char*>(buffer);
	auto length = std::strlen(value);
	pack_value(buffer, length);
	pack_bytes(buffer, value, length);
}

inline void pack_type(buffer_t& buffer, const std::string& value)
{
	pack_type_tag<std::string>(buffer);
	pack_value(buffer, value.length());
	pack_bytes(buffer, value.data(), value.length());
}

template<typename... T>
auto pack(T&&... value)
{
	auto buffer = buffer_t();
	auto buffer_size = ((packed_size(value)) + ...);
	buffer.reserve(buffer_size);
	(pack_type(buffer, value) , ...);
	return buffer;
}



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
decltype(auto) value_cast(buffer_iterator_t& it)
{
	using value_ptr_t = T*;
	return(*value_ptr_t(&*it));
}

template<typename T>
auto unpack_value(buffer_iterator_t& it)
{
	using value_t = std::decay_t<decltype(transform_value_pack(value_cast<T>(it)))>;
	decltype(auto) value = value_cast<value_t>(it);
	decltype(auto) transformed = transform_value_unpack(value);
	auto walk = buffer_walker_t(it, sizeof(value_t));
	return transformed;
}

template<typename T>
void check_type(type_tag_t other)
{
	using value_t = decltype(transform_value_pack(std::declval<T>()));
	auto tag = get_type_tag<value_t>();
	if(tag != other)
		throw std::bad_typeid();
}

template<typename T>
void unpack_and_check_type(buffer_iterator_t& it)
{
	auto tag = unpack_value<type_tag_t>(it);
	check_type<T>(tag);
}

template<typename T>
auto unpack_type(buffer_iterator_t& it)
{
	unpack_and_check_type<T>(it);
	return unpack_value<T>(it);
}

template<>
auto unpack_type<std::string>(buffer_iterator_t& it)
{
	unpack_and_check_type<std::string>(it);
	auto length = unpack_value<std::size_t>(it);
	auto walk = buffer_walker_t(it, length);
	return std::string(std::string::pointer(&*it), length);
}

template<typename... T>
auto unpack(const buffer_t& buffer)
{
	auto finger = std::begin(buffer);
	return std::tuple<T...>((unpack_type<T>(finger))...);
}



int main()
{
	using namespace std;

	auto sb1 = pack(std::size_t(20));
	auto ut1 = unpack<std::size_t>(sb1);

	auto sb2 = pack("this is a hardcoded string");
	auto ut2 = unpack<std::string>(sb2);

	auto sb3 = pack('C', '+', '+', 11, 14, 17, "this is a hardcoded string", "this is std::string"s, N(20), N(23));
	auto ut3 = unpack<char, char, char, int, int, int, std::string, std::string, N, N>(sb3);

	return rand();
}
