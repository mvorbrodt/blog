#pragma once

#include <vector>
#include <functional>

template<typename key, typename hash = std::hash<key>>
class bloom_filter
{
public:
	bloom_filter(uint64_t size) : m_bits(size) {}

	void add(const key& data)
	{
		m_bits[m_hash(data) % m_bits.size()] = true;
	}

	bool contains(const key& data)
	{
		return m_bits[m_hash(data) % m_bits.size()];
	}

private:
	hash m_hash;
	std::vector<bool> m_bits;
};
