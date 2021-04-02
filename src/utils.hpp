#pragma once

#include <vector>

template<typename T, typename... A>
inline auto make_N_of_T(std::size_t N, A&&... a)
{
	std::vector<T> v;
	v.reserve(N);
	for(std::size_t i = 0; i < N; ++i)
		v.emplace_back(std::forward<A>(a)...);
	return v;
}
