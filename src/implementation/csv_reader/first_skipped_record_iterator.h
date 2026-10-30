#ifndef __CSV_READER_FIRST_SKIPPED_RECORD_ITERATOR_H__
#define __CSV_READER_FIRST_SKIPPED_RECORD_ITERATOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <interface/csv_reader/record_iterator_interface.h>

namespace csv_reader
{
    class FirstSkippedRecordIterator: public virtual csv_reader::RecordIteratorInterface
    {
        private:

            std::unique_ptr<csv_reader::RecordIteratorInterface> base;
            bool skipped_first_record;

        public:

            inline FirstSkippedRecordIterator(std::unique_ptr<csv_reader::RecordIteratorInterface> base) noexcept: base(std::move(base)),
                                                                                                                   skipped_first_record(false){}

            inline auto next() -> std::optional<std::string>
            {
                if (this->skipped_first_record)
                {
                    return this->base->next();
                }
                
                std::optional<std::string> first_record = this->base->next();
                this->skipped_first_record = true;

                return this->base->next();
            }
    };
}

#endif