#ifndef __FILE_AS_MEMORY_SPAN_INTERFACE_H__
#define __FILE_AS_MEMORY_SPAN_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <filesystem>
#include <interface/memory/readable_memory_span_interface.h>

namespace file_reader
{
    class FileAsMemorySpanFactoryInterface
    {
        public:

            virtual ~FileAsMemorySpanFactoryInterface() noexcept = default;

            virtual auto to_memory_span(const std::filesystem::path& file_path) -> std::shared_ptr<memory::ReadableMemorySpanInterface> = 0;
    };
}

#endif