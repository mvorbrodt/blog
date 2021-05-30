#pragma once

#include <new>
#include <vector>
#include <utility>
#include <stdexcept>
#include <ostream>
#include <cstddef>
#include <cstdlib>
#ifndef NDEBUG
#include <iostream>
#include <iomanip>
#endif

/// TODO: ===-----------------------------------------------------------===
/// TODO: ===--- Should those be template or constructor parameters? ---===
/// TODO: ===-----------------------------------------------------------===

template<std::size_t ChunkSize, std::size_t ChunksPerBlock = 32>
class memory_pool
{
public:
	static_assert(ChunkSize >= sizeof(void*));
	static_assert(ChunksPerBlock > 0);

	explicit memory_pool() = default;

	memory_pool(const memory_pool&) = delete;

	memory_pool(memory_pool&& other)
	: m_chunks{ std::exchange(other.m_chunks, 0) },
	m_next_chunk{ std::exchange(other.m_next_chunk, nullptr) },
	m_allocated_blocks{ std::move(other.m_allocated_blocks) } {}

	memory_pool& operator = (memory_pool other)
	{
		swap(other);
		return *this;
	}

	~memory_pool()
	{
		for(auto block : m_allocated_blocks)
			std::free(block);
	}

	void* malloc()
	{
		if(!m_next_chunk)
			m_next_chunk = allocate_block(nullptr);

		chunk_ptr_t chunk = m_next_chunk;
		m_next_chunk = chunk->m_next;
		++m_chunks;
#ifndef NDEBUG
		std::cout << "malloc chunk #" << std::setw(2) << std::setfill('0') << chunk->m_seq << " " << chunk << std::endl;
#endif
		return chunk;
	}

	void free(void* ptr)
	{
		chunk_ptr_t chunk = reinterpret_cast<chunk_ptr_t>(ptr);

		chunk->m_next = m_next_chunk;
		m_next_chunk = chunk;
		--m_chunks;
#ifndef NDEBUG
		std::cout << "free   chunk #" << std::setw(2) << std::setfill('0') << m_next_chunk->m_seq << " " << ptr << std::endl;
#endif
	}

	bool empty() const { return !allocated_chunks(); }
	bool full() const { return allocated_chunks() == chunk_capacity(); }

	std::size_t allocated_chunks() const { return m_chunks; }
	std::size_t allocated_blocks() const { return m_allocated_blocks.size(); }

	std::size_t chunks_per_block() const { return ChunksPerBlock; }

	std::size_t chunk_size() const { return ChunkSize; }
	std::size_t block_size() const { return ChunkSize * ChunksPerBlock; }

	void reserve_blocks(std::size_t blocks)
	{
		m_allocated_blocks.reserve(blocks);
		while(blocks > m_allocated_blocks.size())
			m_next_chunk = allocate_block(m_next_chunk);
	}

	std::size_t chunk_capacity() const { return m_allocated_blocks.size() * ChunksPerBlock; }

private:
	struct chunk_t
	{
		chunk_t(chunk_t* next = nullptr) noexcept
		: m_next{ next }
		{
#ifndef NDEBUG
			std::cout << "create chunk #" << std::setw(2) << std::setfill('0') << m_seq << " " << this << " -> " << next << std::endl;
#endif
		}

		union
		{
			chunk_t*  m_next;
			std::byte m_storage[ChunkSize];
		};
#ifndef NDEBUG
		std::size_t m_seq = ++k_seq;
		inline static std::size_t k_seq = 0;
#endif
	};

	using chunk_ptr_t = chunk_t*;

	chunk_ptr_t allocate_block(chunk_ptr_t tail)
	{
		chunk_ptr_t block = reinterpret_cast<chunk_ptr_t>(std::malloc(sizeof(chunk_t) * ChunksPerBlock));
		if(!block)
			throw std::bad_alloc();

		m_allocated_blocks.push_back(block);

		chunk_ptr_t chunk = block;

		for(std::size_t i = 0; i < ChunksPerBlock - 1; ++i)
		{
			new (chunk) chunk_t{ chunk + 1 };
			chunk = chunk->m_next;
		}
		new (chunk) chunk_t{ tail };

		return block;
	}

	void swap(memory_pool& other)
	{
		std::swap(m_chunks, other.m_chunks);
		std::swap(m_next_chunk, other.m_next_chunk);
		std::swap(m_allocated_blocks, other.m_allocated_blocks);
	}

	std::size_t m_chunks = 0;
	chunk_ptr_t m_next_chunk = nullptr;

	using block_list_t = std::vector<chunk_ptr_t>;
	block_list_t m_allocated_blocks;
};

template<std::size_t ChunkSize, std::size_t ChunksPerBlock>
inline std::ostream& operator << (std::ostream& os, const memory_pool<ChunkSize, ChunksPerBlock>& pool)
{
	os << "CS: " << pool.chunk_size() << ", BS: " << pool.block_size() << ", CPB: " << pool.chunks_per_block()
		<< ", chunks: " << pool.allocated_chunks() << ", blocks: " << pool.allocated_blocks() << ", chunk capacit: " << pool.chunk_capacity()
		<< ", full: " << (pool.full() ? "yes" : "no");
	return os;
}
