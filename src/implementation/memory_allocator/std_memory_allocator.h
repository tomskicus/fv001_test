#ifndef __MEMORY_ALLOCATOR_STD_MEMORY_ALLOCATOR_H__
#define __MEMORY_ALLOCATOR_STD_MEMORY_ALLOCATOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <interface/memory_allocator/memory_allocator_interface.h>

namespace memory_allocator
{
    class StdMemoryAllocator: public virtual MemoryAllocatorInterface
    {
        public:

            inline auto malloc(size_t sz) -> void *
            {
                if (sz == 0u)
                {
                    return nullptr;
                }

                void * rs   = std::malloc(sz);

                if (rs == nullptr)
                {
                    throw std::bad_alloc();
                }

                return rs;
            }

            inline void free(void * ptr) noexcept
            {
                if (ptr == nullptr)
                {
                    return;
                }

                std::free(ptr);
            }
    };
}

#endif