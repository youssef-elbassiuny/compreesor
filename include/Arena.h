#pragma once
#include<cstddef>
#include<cstdlib>
#include<memory>
#include<utility>
class Arena_allocator
{
public:
    Arena_allocator(size_t bytes)
        :m_size(bytes)
    {
        m_buffer=static_cast<std::byte*>(malloc(bytes));
        m_offset=m_buffer;
    }
    template<typename T, typename... Args>
    T* alloc(Args&&... args)
    {
        void* offset = m_offset;
        m_offset += sizeof(T);

        T* ptr = static_cast<T*>(offset);

        return std::construct_at(
            ptr,
            std::forward<Args>(args)...
        );
    }
    Arena_allocator(Arena_allocator& other)=delete;
    Arena_allocator operator=(Arena_allocator& other)=delete;
    ~Arena_allocator()
    {
        free(m_buffer);
    }
private:
    size_t m_size=0;
    std::byte* m_buffer;
    std::byte* m_offset;
};
