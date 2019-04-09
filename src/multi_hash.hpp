#pragma once

#include <array>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>

template<typename T>
auto hashN(const T& key, std::size_t N) -> std::vector<std::size_t>
{
	std::minstd_rand0 rng(std::hash<T>{}(key));
	std::vector<std::size_t> hashes(N);
	std::generate(std::begin(hashes), std::end(hashes), rng);
	return hashes;
}

template<std::size_t N, typename T>
auto hashNT(const T& key) -> std::array<std::size_t, N>
{
	std::minstd_rand0 rng(std::hash<T>{}(key));
	std::array<std::size_t, N> hashes{};
	std::generate(std::begin(hashes), std::end(hashes), rng);
	return hashes;
}
