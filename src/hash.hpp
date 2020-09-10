#pragma once

#include <array>
#include <vector>
#include <random>
#include <algorithm>
#include <functional>
#include <climits>
#include <cstdint>

static_assert(
	((SIZE_MAX == 0xFFFFFFFF) || (SIZE_MAX == 0xFFFFFFFFFFFFFFFF)) &&
	((sizeof(std::size_t) * CHAR_BIT) == 32) || ((sizeof(std::size_t) * CHAR_BIT) == 64),
	"std::size_t is neither 32 nor 64 bit");

#if SIZE_MAX == 0xFFFFFFFF
	using random_generator_bits_t = std::mt19937;
#elif SIZE_MAX == 0xFFFFFFFFFFFFFFFF
	using random_generator_bits_t = std::mt19937_64;
#else
	#error "std::size_t is neither 32 nor 64 bit"
#endif

template<typename V = std::size_t, typename K>
inline auto hashN(const K& key, std::size_t N)
{
	random_generator_bits_t rng(std::hash<K>{}(key));
	std::vector<V> hashes(N);
	std::generate(std::begin(hashes), std::end(hashes), rng);
	return hashes;
}

template<std::size_t N, typename V = std::size_t, typename K>
inline auto hashNT(const K& key)
{
	random_generator_bits_t rng(std::hash<K>{}(key));
	std::array<V, N> hashes{};
	std::generate(std::begin(hashes), std::end(hashes), rng);
	return hashes;
}
