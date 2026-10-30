#ifndef __CSV_READER_CSV_READER_INTERFACE_H__
#define __CSV_READER_CSV_READER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <optional>
#include "record_iterator_interface.h"

namespace csv_reader
{
    class CsvReaderInterface
    {
        public:

            virtual ~CsvReaderInterface() noexcept = default;

            virtual void set_file_path(const std::filesystem::path&) = 0;

            virtual void set_read_position(std::optional<size_t>) = 0;
            virtual void set_read_size(std::optional<size_t>) = 0; 

            virtual void set_delimiter(char) = 0;

            virtual auto get_record_iterator() -> std::unique_ptr<RecordIteratorInterface> = 0;
    };
}
#endif