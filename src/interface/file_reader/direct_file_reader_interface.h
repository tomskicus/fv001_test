#ifndef __FILE_READER_FILE_READER_INTERFACE_H__
#define __FILE_READER_FILE_READER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <filesystem>
#include <optional>
#include <memory>
#include <interface/memory/memory_iterator_interface.h>

namespace file_reader
{
    class DirectFileReaderInterface
    {
        public:

            virtual ~DirectFileReaderInterface() noexcept = default;

            virtual auto read(const std::filesystem::path& file_path,
                              std::optional<size_t> offset,
                              std::optional<size_t> sz) -> std::unique_ptr<memory::MemoryIteratorInterface> = 0;
    };
}

#endif