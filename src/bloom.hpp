#pragma once

#include <vector>
#include <bitset>
#include <random>
#include <stdexcept>
#include <functional>
#include <cstddef>
#include "hash.hpp"

template <typename Key>
class bloom_filter
{
public:
	explicit bloom_filter(std::size_t size, std::size_t hash_count = 3)
	: m_bits(size), m_hash_count(hash_count)
	{
		if(!size) throw std::invalid_argument("Size must be greater than zero!");
		if(!hash_count) throw std::invalid_argument("Hash count must be greater than zero!");
	}

	void add(const Key& key) noexcept
	{
		auto hs = hashN(key, m_hash_count);
		for(auto h : hs)
			m_bits[h % m_bits.size()] = true;
	}

	bool contains(const Key& key) const noexcept
	{
		auto hs = hashN(key, m_hash_count);
		for(auto h : hs)
			if(!m_bits[h % m_bits.size()])
				return false;
		return true;
	}

private:
	std::vector<bool> m_bits;
	std::size_t m_hash_count;
};

template <typename Key, std::size_t Size, std::size_t HashCount = 3>
class fixed_bloom_filter
{
public:
	static_assert(Size > 0, "Size must be greater than zero!");
	static_assert(HashCount > 0, "Hash count must be greater than zero!");

	void add(const Key& key)
	{
		auto hs = hashN<HashCount>(key);
		for(auto h : hs)
			m_bits[h % Size] = true;
	}

	bool contains(const Key& key) const
	{
		auto hs = hashN<HashCount>(key);
		for(auto h : hs)
			if(!m_bits[h % Size])
				return false;
		return true;
	}

private:
	std::bitset<Size> m_bits;
};
