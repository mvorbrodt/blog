#pragma once

#include <new>
#include <list>
#include <utility>
#include <stdexcept>
#include <ostream>
#include <iomanip> /// TODO: remove me when removing tracing
#include <cstddef>
#include <cstdlib>

template<typename T>
class memory_pool
{
public:
	explicit memory_pool(std::size_t chunks_per_block)
	: m_chunks_per_block{ chunks_per_block }
	{
		if(!m_chunks_per_block)
			throw std::invalid_argument("chunks_per_block must be greater than zero!");
	}

	memory_pool(const memory_pool&) = delete;

	memory_pool(memory_pool&& other)
	: m_blocks{ std::exchange(other.m_blocks, 0) }, m_chunks{ std::exchange(other.m_chunks, 0) },
	m_chunks_per_block{ std::exchange(other.m_chunks_per_block, 0) }, m_next_chunk{ std::exchange(other.m_next_chunk, nullptr) },
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

	T* malloc()
	{
		if(!m_next_chunk)
			m_next_chunk = allocate_block(nullptr);

		chunk_ptr_t chunk = m_next_chunk;
		m_next_chunk = chunk->m_next;
		++m_chunks;

		std::cout << "malloc chunk #" << std::setw(2) << std::setfill('0') << chunk->m_seq << " " << chunk << std::endl;

		return reinterpret_cast<T*>(chunk);
	}

	void free(T* ptr)
	{
		chunk_ptr_t chunk = reinterpret_cast<chunk_ptr_t>(ptr);

		chunk->m_next = m_next_chunk;
		m_next_chunk = chunk;
		--m_chunks;

		std::cout << "free   chunk #" << std::setw(2) << std::setfill('0') << m_next_chunk->m_seq << " " << ptr << std::endl;
	}

	bool empty() const { return !count(); }

	bool full() const { return count() == capacity(); }

	std::size_t count() const { return m_chunks; }

	std::size_t capacity() const { return m_blocks * m_chunks_per_block; }

	std::size_t block_size() const { return m_chunks_per_block; }

	void reserve(std::size_t blocks)
	{
		while(blocks > m_blocks)
			m_next_chunk = allocate_block(m_next_chunk);
	}

private:
	struct chunk_t
	{
		chunk_t(chunk_t* next = nullptr) noexcept
		: m_next{ next }
		{
			std::cout << "create chunk #" << std::setw(2) << std::setfill('0') << m_seq << " " << this << " -> " << next << std::endl;
		}

		union
		{
			chunk_t*  m_next;
			std::byte m_storage[sizeof(T)];
		};

		std::size_t m_seq = ++k_seq;
		inline static std::size_t k_seq = 0;
	};

	using chunk_ptr_t = chunk_t*;

	chunk_ptr_t allocate_block(chunk_ptr_t last_link)
	{
		chunk_ptr_t block = reinterpret_cast<chunk_ptr_t>(std::malloc(m_chunks_per_block * sizeof(chunk_t)));
		if(!block)
			throw std::bad_alloc();

		m_allocated_blocks.push_back(block);

		chunk_ptr_t chunk = block;

		for(std::size_t i = 0; i < m_chunks_per_block - 1; ++i)
		{
			new (chunk) chunk_t{ chunk + 1 };
			chunk = chunk->m_next;
		}
		new (chunk) chunk_t{ last_link };

		++m_blocks;

		return block;
	}

	void swap(memory_pool& other)
	{
		std::swap(m_blocks, other.m_blocks);
		std::swap(m_chunks, other.m_chunks);
		std::swap(m_chunks_per_block, other.m_chunks_per_block);
		std::swap(m_next_chunk, other.m_next_chunk);
		std::swap(m_allocated_blocks, other.m_allocated_blocks);
	}

	std::size_t m_blocks = 0;
	std::size_t m_chunks = 0;
	std::size_t m_chunks_per_block;
	chunk_ptr_t m_next_chunk = nullptr;

	using block_list_t = std::list<void*>;
	block_list_t m_allocated_blocks;
};

template<typename T>
inline std::ostream& operator << (std::ostream& os, const memory_pool<T>& pool)
{
	os << "full: " << (pool.full() ? "yes" : "no") << ", count: " << pool.count() << ", capacity: " << pool.capacity() << ", block size: " << pool.block_size();
	return os;
}
