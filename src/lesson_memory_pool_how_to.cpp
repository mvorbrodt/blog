#include <iostream>
#include <iomanip>
#include <utility>
#include <vector>
#include <list>
#include <new>
#include <random>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <cstddef>
#include <cstdlib>

using namespace std;

template<std::size_t ChunkSize, std::size_t ChunksPerBlock>
class memory_pool
{
public:
	static_assert(ChunkSize >= sizeof(void*));
	static_assert(ChunksPerBlock > 0);

	explicit memory_pool() = default;

	memory_pool(const memory_pool&) = delete;
	memory_pool(memory_pool&& other)
	{}

	memory_pool& operator = (memory_pool other)
	{
		return *this;
	}

	~memory_pool()
	{
		for(auto ptr : m_blocks)
			std::free(ptr);
	}

	void* malloc()
	{
		if(m_next_chunk == nullptr)
			m_next_chunk = allocate_block(nullptr);
		chunk_t* chunk = m_next_chunk;
		m_next_chunk = m_next_chunk->m_next;
		cout << "malloc chunk #" << setw(2) << setfill('0') << chunk->m_seq << " " << chunk << endl;
		return chunk;
	}

	void free(void* ptr)
	{
		chunk_t* chunk = reinterpret_cast<chunk_t*>(ptr);
		chunk->m_next = m_next_chunk;
		m_next_chunk = chunk;
		cout << "free   chunk #" << setw(2) << setfill('0') << chunk->m_seq << " " << chunk << endl;
	}

	void reserve_blocks(std::size_t blocks)
	{
		while(blocks > m_blocks.size())
			m_next_chunk = allocate_block(m_next_chunk);
	}

private:
	struct chunk_t
	{
		chunk_t(chunk_t* next = nullptr) : m_next{ next }
		{
			cout << "create chunk #" << setw(2) << setfill('0') << m_seq << " " << this << " -> " << m_next << endl;
		}

		union
		{
			chunk_t* m_next;
			std::byte m_storage[ChunkSize];
		};

		std::size_t m_seq = ++k_seq;
		inline static std::size_t k_seq = 0;
	};

	chunk_t* allocate_block(chunk_t* tail)
	{
		unique_lock guard(m_lock);
		if(m_next_chunk != nullptr)
			return m_next_chunk;

		chunk_t* block = reinterpret_cast<chunk_t*>(std::malloc(sizeof(chunk_t) * ChunksPerBlock));
		if(block == nullptr) throw std::bad_alloc();
		m_blocks.push_back(block);

		chunk_t* chunk = block;

		for(std::size_t i = 0; i < ChunksPerBlock - 1; ++i)
		{
			new (chunk) chunk_t{ chunk + 1 };
			chunk = chunk->m_next;
		}
		new (chunk) chunk_t{ tail };

		return block;
	}

	chunk_t* m_next_chunk = nullptr;

	std::mutex m_lock;
	using block_list_t = std::list<void*>;
	block_list_t m_blocks;
};

int main()
{
	constexpr auto Chunks = 6;

	memory_pool<sizeof(void*), 3> pool;
	vector<void*> chunks;

	pool.reserve_blocks(1);

	for(auto i = 0; i < Chunks; ++i)
	{
		void* chunk = pool.malloc();
		chunks.push_back(chunk);
	}

	random_device dev;
	mt19937 gen{ dev() };

	shuffle(chunks.begin(), chunks.end(), gen);

	for(auto chunk : chunks)
		pool.free(chunk);
}

