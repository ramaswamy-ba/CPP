#include <iostream>
#include <vector>
#include <memory>
class MemoryPool
{
private:
    size_t m_block_size;
    size_t m_block_count;
    size_t m_total_mem;
    void  *m_start_ptr;
    std::vector<void*> m_pool;

    size_t m_alignment;
    void  *m_aligned_ptr;

public:
    MemoryPool(size_t block_size, size_t block_count, size_t alignment=0);
    ~MemoryPool();
    void* allocate();
    void deallocate(void *ptr);
};

MemoryPool::MemoryPool(size_t block_size, size_t block_count, size_t alignment)
           :m_block_size(block_size), m_block_count(block_count), m_alignment(alignment)
{
    m_total_mem = m_block_size * m_block_count + m_alignment;
    m_start_ptr = operator new(m_total_mem);

    m_aligned_ptr = m_start_ptr;
    //static_assert(m_alignment > 0 && ((m_alignment & m_alignment-1) == 0), "alignment should be power of 2");
    if ( m_alignment >  0)
        m_aligned_ptr = std::align(m_alignment, m_block_size, m_start_ptr, m_total_mem);

    for(size_t i =0; i< m_block_count; ++i)
        m_pool.push_back(static_cast<char*>( m_aligned_ptr) + i * m_block_size);
}
MemoryPool::~MemoryPool() { operator delete(m_start_ptr); }
void* MemoryPool::allocate()
{
    if( m_pool.empty() )
        throw std::bad_alloc();
    void *ptr = m_pool.back(); m_pool.pop_back();
    return ptr;
}
void MemoryPool::deallocate(void *ptr)
{
    m_pool.push_back(ptr);
}

int main()
{
    MemoryPool pool(32, 10); // Pool with 10 blocks, each 32 bytes
    void *block1 = pool.allocate();
    void *block2 = pool.allocate();

    int *arr = new(block1) int[8]; //use the memory
    for(int i =0; i<8; ++i )
        arr[i] =  i+1;
    for(int i =0; i<8; ++i )
        std::cout<<arr[i]<<',';
    std::cout<<'\n';
    return 0;
}
