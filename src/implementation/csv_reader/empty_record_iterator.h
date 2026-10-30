#ifndef __CSV_READER_EMPTY_RECORD_ITERATOR_H__
#define __CSV_READER_EMPTY_RECORD_ITERATOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <interface/csv_reader/record_iterator_interface.h>

namespace csv_reader
{
    class EmptyRecordIterator: public virtual csv_reader::RecordIteratorInterface
    {
        public:

            inline auto next() -> std::optional<std::string>
            {
                return std::nullopt;
            }
    };
}

#endif