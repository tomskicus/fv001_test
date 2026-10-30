#ifndef __MEMORY_ALLOCATOR_MEMORY_ALLOCATOR_INTERFACE_H__
#define __MEMORY_ALLOCATOR_MEMORY_ALLOCATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>

namespace memory_allocator
{
    class MemoryAllocatorInterface
    {
        public:

            virtual ~MemoryAllocatorInterface() noexcept = default;

            virtual auto malloc(size_t sz) -> void * = 0;
            virtual void free(void * mem) noexcept = 0;
    };
}

#endif