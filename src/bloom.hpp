#pragma once

#include <vector>
#include <bitset>
#include <random>
#include <stdexcept>
#include <functional>
#include <cstddef>
#include "hash.hpp"

template <typename Key, std::size_t HashN, typename Hash = std::hash<Key>>
class bloom_filter
{
public:
	static_assert(HashN > 0, "HashN must be greater than zero!");

	bloom_filter(std::size_t size)
	: m_bits(size)
	{
		if(!size)
			throw std::invalid_argument("Size must be greater than zero!");
	}

	void add(const Key& key)
	{
		auto hv = hashNT<HashN, std::size_t>(key);
		for(std::size_t i = 0; i < HashN; ++i)
			m_bits[hv[i] % m_bits.size()] = true;
	}

	bool contains(const Key& key) const
	{
		auto hv = hashNT<HashN, std::size_t>(key);
		for(std::size_t i = 0; i < HashN; ++i)
			if(!m_bits[hv[i] % m_bits.size()])
				return false;
		return true;
	}

private:
	std::vector<bool> m_bits;
};

template <typename Key, std::size_t Size, std::size_t HashN, typename Hash = std::hash<Key>>
class fixed_bloom_filter
{
public:
	static_assert(Size > 0, "Size must be greater than zero!");

	void add(const Key& key)
	{
		auto hv = hashNT<HashN, std::size_t>(key);
		for(std::size_t i = 0; i < HashN; ++i)
			m_bits[hv[i] % Size] = true;
	}

	bool contains(const Key& key) const
	{
		auto hv = hashNT<HashN, std::size_t>(key);
		for(std::size_t i = 0; i < HashN; ++i)
			if(!m_bits[hv[i] % Size])
				return false;
		return true;
	}

private:
	std::bitset<Size> m_bits;
};
