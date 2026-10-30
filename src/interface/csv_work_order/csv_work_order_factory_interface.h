#ifndef __CSV_WORK_ORDER_CSV_WORK_ORDER_FACTORY_INTERFACE_H__
#define __CSV_WORK_ORDER_CSV_WORK_ORDER_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <filesystem>
#include "csv_work_order_iterator_interface.h" 

namespace csv_work_order
{
    class CsvWorkOrderFactoryInterface
    {
        public:

            virtual ~CsvWorkOrderFactoryInterface() noexcept = default;

            virtual void set_file_path(const std::filesystem::path&) = 0;
            virtual void set_byte_chunk_size(size_t) = 0;
            virtual void set_delimiter(char) = 0;
            virtual void set_skip_header(bool) = 0;

            virtual auto get() -> std::unique_ptr<CsvWorkOrderIteratorInterface> = 0;
    };
}

#endif