#pragma once

#include <vector>
#include <functional>
#include <cstddef>

template<typename key, typename hash = std::hash<key>>
class bloom_filter
{
public:
	bloom_filter(std::size_t size) : m_bits(size) {}

	void add(const key& data)
	{
		m_bits[hash{}(data) % m_bits.size()] = true;
	}

	bool contains(const key& data)
	{
		return m_bits[hash{}(data) % m_bits.size()];
	}

private:
	std::vector<bool> m_bits;
};
