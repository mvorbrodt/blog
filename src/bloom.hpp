#pragma once

#include <vector>
#include <random>
#include <functional>
#include <cstddef>
#include <cassert>

template<typename Key, typename Hash = std::hash<Key>>
class basic_bloom_filter
{
public:
	basic_bloom_filter(std::size_t size) : m_bits(size) {}

	void add(const Key& key)
	{
		m_bits[Hash{}(key) % m_bits.size()] = true;
	}

	bool contains(const Key& key)
	{
		return m_bits[Hash{}(key) % m_bits.size()];
	}

private:
	std::vector<bool> m_bits;
};

namespace
{
	template<typename Key, typename Hash = std::hash<Key>>
	class mixer
	{
	public:
		mixer(std::size_t size, const Key& key)
		: m_size(size), m_random(Hash{}(key))
		{
			assert(size > 0);
		}

		std::size_t operator()() { return m_random() % m_size; }

	private:
		std::size_t m_size;
		std::mt19937 m_random;
	};
}

template <typename Key, typename Hash = std::hash<Key>>
class bloom_filter
{
public:
	bloom_filter(std::size_t size, std::size_t hashes)
	: m_size(size), m_hashes(hashes), m_bits(size)
	{
		assert(size > 0);
		assert(hashes > 0);
	}

	void add(const Key& key)
	{
		mixer<Key, Hash> m(m_size, key);
		for(std::size_t i = 0; i < m_hashes; ++i)
			m_bits[m()] = true;
	}

	bool contains(const Key& key) const
	{
		mixer<Key, Hash> m(m_size, key);
		for(std::size_t i = 0; i < m_hashes; ++i)
			if(!m_bits[m()]) return false;
		return true;
	}

private:
	std::size_t m_size;
	std::size_t m_hashes;
	std::vector<bool> m_bits;
};
