#pragma once

#include <array>
#include <random>
#include <algorithm>
#include <functional>

template<size_t N, typename T>
auto hashN(const T& key) -> std::array<size_t, N>
{
	std::minstd_rand0 rng(std::hash<T>{}(key));
	std::array<size_t, N> hashes = {};
	std::generate(begin(hashes), end(hashes), rng);
	return hashes;
}
