#pragma once

#include <tuple>
#include <vector>
#include <utility>
#include <iterator>
#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <cstddef>
#include <cstdint>



using byte_t = std::byte;
using buffer_t = std::vector<byte_t>;
using buffer_ptr_t = buffer_t::pointer;
using buffer_input_t = buffer_t::const_iterator;
using buffer_output_t = std::back_insert_iterator<buffer_t>;

struct on_exit_t
{
	using proc_t = std::function<void()>;
	on_exit_t(proc_t&& proc) : m_proc{ std::move(proc) } {}
	~on_exit_t() { m_proc(); }
	proc_t m_proc;
};



inline void pack_bytes(buffer_output_t& it, const void* ptr, std::size_t count);
inline void unpack_bytes(buffer_input_t& it, void* ptr, std::size_t count);

template<typename T> void pack_value(buffer_output_t& it, const T& value);
template<typename T> decltype(auto) unpack_value(buffer_input_t& it);



using type_hash_t = decltype(std::declval<std::type_info>().hash_code());

using pack_transform_t = std::function<void(const void*, buffer_output_t&)>;
using unpack_transform_t = std::function<void(buffer_input_t&, void*)>;

using pack_transform_map_t = std::unordered_map<type_hash_t, pack_transform_t>;
using unpack_transform_map_t = std::unordered_map<type_hash_t, unpack_transform_t>;

using transform_maps_t = std::pair<pack_transform_map_t, unpack_transform_map_t>;

inline decltype(auto) get_transform_maps()
{
	static auto k_transform_maps = transform_maps_t();

	return(k_transform_maps);
}



template<typename T, typename P>
auto add_pack_transform(P proc)
{
	auto& [pack_map, _] = get_transform_maps();
	auto hash = typeid(T).hash_code();
	auto transform = pack_transform_t([=](const void* pv, buffer_output_t& it) { proc(*((const T*)pv), it); });

	auto [it, result] = pack_map.insert_or_assign(hash, transform);
	return result == true;
}

template<typename T>
decltype(auto) get_pack_transform()
{
	using DT = std::decay_t<T>;
	static auto k_default_pack_transform = pack_transform_t([](const void* p, buffer_output_t& it) { pack_value(it, *((const DT*)p)); });

	auto& [pack_map, _] = get_transform_maps();
	auto hash = typeid(DT).hash_code();
	auto it = pack_map.find(hash);
	if(it != std::end(pack_map))
		return((*it).second);

	return(k_default_pack_transform);
}

template<typename T>
auto remove_pack_transform()
{
	auto& [pack_map, _] = get_transform_maps();
	auto hash = typeid(T).hash_code();
	auto result = pack_map.erase(hash);

	return result > 0;
}



template<typename T, typename P>
auto add_unpack_transform(P proc)
{
	auto& [_, unpack_map] = get_transform_maps();
	auto hash = typeid(T).hash_code();
	auto transform = unpack_transform_t([=](buffer_input_t& it, void* pv) { proc(it, (T*)pv); });

	auto [it, result] = unpack_map.insert_or_assign(hash, transform);
	return result == true;
}

template<typename T>
decltype(auto) get_unpack_transform()
{
	static auto k_default_unpack_transform = unpack_transform_t([](buffer_input_t& it, void* p) { new (p) T(unpack_value<T>(it)); });

	auto& [_, unpack_map] = get_transform_maps();
	auto hash = typeid(T).hash_code();
	auto it = unpack_map.find(hash);
	if(it != std::end(unpack_map))
		return((*it).second);

	return(k_default_unpack_transform);
}

template<typename T>
auto remove_unpack_transform()
{
	auto& [_, unpack_map] = get_transform_maps();
	auto hash = typeid(T).hash_code();
	auto result = unpack_map.erase(hash);

	return result > 0;
}



using pack_size_proc_t = std::function<std::size_t(const void*)>;
using pack_size_proc_map_t = std::unordered_map<type_hash_t, pack_size_proc_t>;

inline decltype(auto) get_pack_size_proc_map()
{
	static auto k_pack_size_proc_map = pack_size_proc_map_t();

	return(k_pack_size_proc_map);
}



template<typename T, typename P>
auto add_pack_size_proc(P proc)
{
	decltype(auto) proc_map = get_pack_size_proc_map();
	auto hash = typeid(T).hash_code();
	auto size_proc = pack_size_proc_t([=](const void* pv) { return proc(*((const T*)pv)); });

	auto [it, result] = proc_map.insert_or_assign(hash, size_proc);
	return result == true;
}

template<typename T>
decltype(auto) get_pack_size_proc()
{
	static auto k_default_pack_size_proc = pack_size_proc_t([](const void*) { return sizeof(T); });

	decltype(auto) proc_map = get_pack_size_proc_map();
	auto hash = typeid(T).hash_code();
	auto it = proc_map.find(hash);
	if(it != std::end(proc_map))
		return((*it).second);

	return(k_default_pack_size_proc);
}

template<typename T>
auto remove_pack_size_proc()
{
	decltype(auto) proc_map = get_pack_size_proc_map();
	auto hash = typeid(T).hash_code();
	auto result = proc_map.erase(hash);

	return result > 0;
}



inline void pack_bytes(buffer_output_t& it, const void* ptr, std::size_t count)
{
	std::copy_n(buffer_ptr_t(ptr), count, it);
}

template<typename T>
void pack_value(buffer_output_t& it, const T& value)
{
	pack_bytes(it, &value, sizeof(value));
}

template<typename T>
void pack_type(buffer_output_t& it, T&& value)
{
	decltype(auto) transform = get_pack_transform<T>();
	transform(&value, it);
}

template<typename T>
void pack_type(buffer_output_t& it, const T* ptr)
{
	decltype(auto) transform = get_pack_transform<const T*>();
	transform(&ptr, it);
}

template<typename T>
auto pack_size(T&& value)
{
	auto size_proc = get_pack_size_proc<T>();
	auto type_size = size_proc(&value);
	return type_size;
}

template<typename T>
auto pack_size(const T* ptr)
{
	auto size_proc = get_pack_size_proc<const T*>();
	auto type_size = size_proc(&ptr);
	return type_size;
}

template<typename... T>
auto pack(T&&... value)
{
	auto buffer = buffer_t();
	auto buffer_size = ((pack_size(std::forward<T>(value)))+...);
	buffer.reserve(buffer_size);

	auto it = buffer_output_t(buffer);
	(pack_type(it, std::forward<T>(value)),...);

	return buffer;
}



template<typename T>
decltype(auto) value_cast(const void* ptr)
{
	using value_ptr_t = const T*;
	return(*value_ptr_t(ptr));
}

inline void unpack_bytes(buffer_input_t& it, void* ptr, std::size_t count)
{
	auto walk_it = on_exit_t([&]() { it += count; });
	std::copy_n(it, count, buffer_ptr_t(ptr));
}

template<typename T>
decltype(auto) unpack_value(buffer_input_t& it)
{
	auto walk_it = on_exit_t([&]() { it += sizeof(T); });
	return(value_cast<T>(&*it));
}

template<typename T>
auto unpack_type(buffer_input_t& it)
{
	byte_t storage[sizeof(T)] = {};
	auto clean_it = on_exit_t([&]() { value_cast<T>(storage).~T(); });

	decltype(auto) transform = get_unpack_transform<T>();
	transform(it, storage);

	return value_cast<T>(storage);
}

template<typename... T>
auto unpack(const buffer_t& buffer)
{
	auto it = std::begin(buffer);
	return std::tuple<T...>((unpack_type<T>(it))...);
}
