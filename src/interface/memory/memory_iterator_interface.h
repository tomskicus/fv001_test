#ifndef __MEMORY_MEMORY_ITERATOR_INTERFACE_H__
#define __MEMORY_MEMORY_ITERATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <optional>
#include <string>

namespace memory
{
    class MemoryIteratorInterface
    {
        public:

            virtual ~MemoryIteratorInterface() noexcept = default;

            virtual auto next(size_t byte_sz) -> std::optional<std::string> = 0;
    };
}

#endif