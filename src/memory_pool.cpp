#include <iostream>
#include <algorithm>
#include <random>
#include <vector>
#include "memory_pool.hpp"

struct T
{
	int m_int = ++k_seq;
	inline static int k_seq = 0;
};

int main()
{
	using namespace std;

	auto I = 3;
	auto BLOCKS = 3;
	auto CHUNKS_PER_BLOCK = 2;
	auto CHUNKS_TO_MALLOC = CHUNKS_PER_BLOCK * BLOCKS;
	auto BLOCK_RESERVE = 1;

	using chunks_t = std::vector<T*>;

	auto chunks = chunks_t{};
	auto gen = std::mt19937{ (std::random_device{})() };

	auto pool = memory_pool<T>(CHUNKS_PER_BLOCK);
	pool.reserve(BLOCK_RESERVE);

	cout << pool << endl << endl;

	for(auto i = 0; i < I; ++i)
	{
		for(auto c = 0; c < CHUNKS_TO_MALLOC - i; ++c)
			chunks.push_back(pool.malloc());
		cout << pool << endl << endl;

		std::shuffle(std::begin(chunks), std::end(chunks), gen);

		for(auto chunk : chunks)
			pool.free(chunk);
		cout << pool << endl << endl;

		chunks.clear();
	}
}
