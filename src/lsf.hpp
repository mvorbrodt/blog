#pragma once

#include <any>
#include <tuple>
#include <vector>
#include <utility>
#include <iterator>
#include <typeinfo>
#include <functional>
#include <unordered_map>
#include <cstddef>
#include <cstdint>
#include <cassert>



class serializer;

using byte_t = std::byte;
using buffer_t = std::vector<byte_t>;
using buffer_ptr_t = buffer_t::pointer;
using buffer_input_t = buffer_t::const_iterator;
using buffer_output_t = std::back_insert_iterator<buffer_t>;

using pack_transform_t = std::function<void(const serializer&, const void*, buffer_output_t&)>;
using unpack_transform_t = std::function<std::any(const serializer&, buffer_input_t&)>;
using pack_size_proc_t = std::function<std::size_t(const serializer&, const void*)>;



template<typename T>
struct byte_cast_buffer_input_t : std::iterator<std::random_access_iterator_tag, T>
{
	static_assert(sizeof(T) == sizeof(buffer_input_t::value_type));

	using this_t = byte_cast_buffer_input_t;

	byte_cast_buffer_input_t(buffer_input_t& it) : m_it{ it } {}
	byte_cast_buffer_input_t(const buffer_input_t& it) : m_it{ const_cast<buffer_input_t&>(it) } {}

	const T& operator * () const { return reinterpret_cast<const T&>(*m_it); }

	this_t& operator ++ () { ++m_it; return *this; }
	this_t operator ++ (int) { auto temp = *this; ++(*this); return temp; }

	friend auto operator - (const this_t& lhs, const this_t& rhs) { return lhs.m_it - rhs.m_it; }

	friend bool operator == (const this_t& lhs, const this_t& rhs) { return lhs.m_it == rhs.m_it; }
	friend bool operator != (const this_t& lhs, const this_t& rhs) { return !(lhs == rhs); }

private:
	buffer_input_t& m_it;
};



struct on_exit_t
{
	using proc_t = std::function<void()>;

	on_exit_t(proc_t&& proc, bool do_it = true)
	: m_proc{ std::forward<proc_t>(proc) }, m_do_it{ do_it } {}

	~on_exit_t() { if(m_do_it) m_proc(); }

	void do_it() noexcept { m_do_it = true; }
	void do_not() noexcept { m_do_it = false; }

private:
	proc_t m_proc;
	bool m_do_it;
};



class serializer
{
public:
	template<typename T, typename P>
	auto add_pack_transform(P proc)
	{
		auto hash = typeid(T).hash_code();
		auto transform = pack_transform_t([=](const serializer& s, const void* pv, buffer_output_t& it)
			{ proc(s, *((const T*)pv), it); });

		auto [it, result] = m_pack_transforms.insert_or_assign(hash, transform);
		return result == true;
	}

	template<typename T>
	auto remove_pack_transform()
	{
		auto hash = typeid(T).hash_code();
		auto result = m_pack_transforms.erase(hash);

		return result > 0;
	}



	template<typename T, typename P>
	auto add_unpack_transform(P proc)
	{
		auto hash = typeid(T).hash_code();
		auto transform = unpack_transform_t([=](const serializer& s, buffer_input_t& it)
			{ return std::make_any<T>(proc(s, it)); });

		auto [it, result] = m_unpack_transforms.insert_or_assign(hash, transform);
		return result == true;
	}

	template<typename T>
	auto remove_unpack_transform()
	{
		auto hash = typeid(T).hash_code();
		auto result = m_unpack_transforms.erase(hash);

		return result > 0;
	}



	template<typename T, typename P>
	auto add_pack_size_proc(P proc)
	{
		auto hash = typeid(T).hash_code();
		auto size_proc = pack_size_proc_t([=](const serializer& s, const void* pv)
			{ return proc(s, *((const T*)pv)); });

		auto [it, result] = m_pack_size_procs.insert_or_assign(hash, size_proc);
		return result == true;
	}

	template<typename T>
	auto remove_pack_size_proc()
	{
		auto hash = typeid(T).hash_code();
		auto result = m_pack_size_procs.erase(hash);

		return result > 0;
	}



	void pack_bytes(buffer_output_t& it, const void* ptr, std::size_t count) const
	{
		std::copy_n(buffer_ptr_t(ptr), count, it);
	}

	template<typename T>
	void pack_value(buffer_output_t& it, const T& value) const
	{
		pack_bytes(it, &value, sizeof(value));
	}

	template<typename T>
	void pack_type(buffer_output_t& it, T&& value) const
	{
		decltype(auto) transform = get_pack_transform<T>();
		transform(*this, &value, it);
	}

	template<typename T>
	void pack_type(buffer_output_t& it, const T* ptr) const
	{
		decltype(auto) transform = get_pack_transform<const T*>();
		transform(*this, &ptr, it);
	}

	template<typename T>
	auto pack_size(T&& value) const
	{
		auto size_proc = get_pack_size_proc<T>();
		auto type_size = size_proc(*this, &value);
		return type_size;
	}

	template<typename T>
	auto pack_size(const T* ptr) const
	{
		auto size_proc = get_pack_size_proc<const T*>();
		auto type_size = size_proc(*this, &ptr);
		return type_size;
	}

	template<typename... T>
	auto pack(T&&... value) const
	{
		auto buffer = buffer_t();
		auto buffer_size = ((pack_size(std::forward<T>(value)))+...);
		buffer.reserve(buffer_size);

		auto it = buffer_output_t(buffer);
		(pack_type(it, std::forward<T>(value)),...);

		assert(buffer_size == buffer.size());

		return buffer;
	}



	template<typename T>
	decltype(auto) value_cast(const void* ptr) const
	{
		using value_ptr_t = T*;
		return(*value_ptr_t(ptr));
	}

	void unpack_bytes(buffer_input_t& it, void* ptr, std::size_t count) const
	{
		auto walk_it = on_exit_t([&]() { it += count; });
		std::copy_n(it, count, buffer_ptr_t(ptr));
	}

	template<typename T>
	decltype(auto) unpack_value(buffer_input_t& it) const
	{
		auto walk_it = on_exit_t([&]() { it += sizeof(T); });
		return(value_cast<T>(&*it));
	}

	template<typename T>
	auto unpack_type(buffer_input_t& it) const
	{
		decltype(auto) transform = get_unpack_transform<T>();
		return std::any_cast<T>(transform(*this, it));
	}

	template<typename... T>
	auto unpack(const buffer_t& buffer) const
	{
		auto it = std::begin(buffer);
		return std::tuple<T...>((unpack_type<T>(it))...);
	}

private:
	using type_hash_t = decltype(std::declval<std::type_info>().hash_code());
	using pack_transform_map_t = std::unordered_map<type_hash_t, pack_transform_t>;
	using unpack_transform_map_t = std::unordered_map<type_hash_t, unpack_transform_t>;
	using pack_size_proc_map_t = std::unordered_map<type_hash_t, pack_size_proc_t>;

	template<typename T>
	decltype(auto) get_pack_transform() const
	{
		using DT = std::decay_t<T>;
		static const auto k_default_pack_transform = pack_transform_t([](const serializer& s, const void* p, buffer_output_t& it)
			{ s.pack_value(it, *((const DT*)p)); });

		auto hash = typeid(DT).hash_code();
		auto it = m_pack_transforms.find(hash);
		if(it != std::end(m_pack_transforms))
			return((*it).second);

		return(k_default_pack_transform);
	}

	template<typename T>
	decltype(auto) get_unpack_transform() const
	{
		static const auto k_default_unpack_transform = unpack_transform_t([](const serializer& s, buffer_input_t& it)
			{ return std::make_any<T>(s.unpack_value<T>(it)); });

		auto hash = typeid(T).hash_code();
		auto it = m_unpack_transforms.find(hash);
		if(it != std::end(m_unpack_transforms))
			return((*it).second);

		return(k_default_unpack_transform);
	}

	template<typename T>
	decltype(auto) get_pack_size_proc() const
	{
		static const auto k_default_pack_size_proc = pack_size_proc_t([](const serializer&, const void*)
			{ return sizeof(T); });

		auto hash = typeid(T).hash_code();
		auto it = m_pack_size_procs.find(hash);
		if(it != std::end(m_pack_size_procs))
			return((*it).second);

		return(k_default_pack_size_proc);
	}

	pack_transform_map_t m_pack_transforms;
	unpack_transform_map_t m_unpack_transforms;
	pack_size_proc_map_t m_pack_size_procs;
};
