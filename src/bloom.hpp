#pragma once

#include <vector>
#include <functional>

template<typename key, typename hash = std::hash<key>>
class BloomFilter
{
public:
	BloomFilter(uint64_t size) : m_bits(size) {}

	void add(const key& data)
	{
		auto bits = hash{}(data);
		m_bits[bits % m_bits.size()] = true;
	}

	bool contains(const std::string& data)
	{
		auto bits = hash{}(data);
		return m_bits[bits % m_bits.size()];
	}

private:
	std::vector<bool> m_bits;
};
