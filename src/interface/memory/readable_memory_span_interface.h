#ifndef __MEMORY_IMMUTABLE_MEMORY_SPAN_INTERFACE_H__
#define __MEMORY_IMMUTABLE_MEMORY_SPAN_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>

namespace memory
{
    class ReadableMemorySpanInterface
    {
        public:

            virtual ~ReadableMemorySpanInterface() noexcept = default;

            virtual auto read(size_t offset, size_t sz) -> std::string = 0;
            virtual auto size() -> size_t = 0;
    };
}

#endif