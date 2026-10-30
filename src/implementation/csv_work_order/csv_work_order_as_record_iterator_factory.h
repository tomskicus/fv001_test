#ifndef __CSV_WORK_ORDER_CSV_WORK_ORDER_AS_RECORD_ITERATOR_FACTORY_H__
#define __CSV_WORK_ORDER_CSV_WORK_ORDER_AS_RECORD_ITERATOR_FACTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <optional>
#include <implementation/csv_reader/csv_reader.h>
#include <implementation/csv_pos_seeker/delimiter_owned_pos_seeker.h>
#include <implementation/file_reader/linux_random_access_file_reader.h>
#include <exception>
#include <stdexcept>
#include <interface/csv_work_order/csv_work_order_as_record_iterator_factory_interface.h>
#include <fstream>

#include <implementation/csv_reader/empty_record_iterator.h>
#include <implementation/csv_reader/first_skipped_record_iterator.h>

namespace csv_work_order
{
    class CsvWorkOrderAsRecordIteratorFactory: public virtual CsvWorkOrderAsRecordIteratorFactoryInterface
    {
        private:

            std::unique_ptr<csv_reader::CsvReaderInterface> csv_reader;
            std::unique_ptr<csv_pos_seeker::DelimiterOwnedPosSeekerInterface> csv_pos_seeker;
            std::unique_ptr<file_reader::FileAsMemorySpanFactoryInterface> file_vm_factory; 

        public:

            inline CsvWorkOrderAsRecordIteratorFactory(): csv_reader(std::make_unique<csv_reader::CsvReader>()),
                                                          csv_pos_seeker(std::make_unique<csv_pos_seeker::DelimiterOwnedPosSeeker>()),
                                                          file_vm_factory(std::make_unique<file_reader::LinuxRandomAccessFileReader>()){}

            inline auto to_record_iterator(const model::CSVWorkOrder& csv_work_order) -> std::unique_ptr<csv_reader::RecordIteratorInterface>
            {
                size_t file_sz  = this->get_file_size(csv_work_order.file_path);

                size_t first    = csv_work_order.read_off;
                size_t last     = first + csv_work_order.read_sz;

                if (last < first)
                {
                    throw std::invalid_argument("bad csv workorder segment, last < first");
                }

                if (first > file_sz)
                {
                    throw std::invalid_argument("bad csv workorder segment, first > file size");
                }

                std::shared_ptr<memory::ReadableMemorySpanInterface> readable_memory = this->file_vm_factory->to_memory_span(csv_work_order.file_path);

                //assume that we are working on a positive segment
                //assume that we are not working on a 0 segment
                //both cases, last == file_sz indicates that the ownership of the last record belongs to that uniqueness
                //our question is which work order should be responsible for the last record
                //and it seems reasonable that the workorder that has last == file_sz should be it, because it is unique, and it is guaranteed, and it is contiguous

                if (last == file_sz)
                {
                    readable_memory = std::make_unique<LastDelimiterExtendedMemorySpan>
                    (
                        std::move(readable_memory),
                        csv_work_order.delimiter
                    );

                    last = last + 1;
                }

                this->csv_pos_seeker->set_memory_span(readable_memory);
                this->csv_pos_seeker->set_record_delimiter(csv_work_order.delimiter);
                this->csv_pos_seeker->set_offset(first);
                this->csv_pos_seeker->set_size(last - first);

                std::optional<std::pair<size_t, size_t>> owned_segment  = this->csv_pos_seeker->get_owned_segment();

                if (!owned_segment.has_value())
                {
                    return std::make_unique<csv_reader::EmptyRecordIterator>();
                }

                auto [off, sz]          = owned_segment.value();

                this->csv_reader->set_file_path(csv_work_order.file_path);
                this->csv_reader->set_read_position(off);
                this->csv_reader->set_read_size(sz);
                this->csv_reader->set_delimiter(csv_work_order.delimiter);

                std::unique_ptr<csv_reader::RecordIteratorInterface> record_iterator    = this->csv_reader->get_record_iterator(); 

                if (off == 0u && csv_work_order.skip_header) //ownership of first record (guaranteed)
                {
                    record_iterator = std::make_unique<csv_reader::FirstSkippedRecordIterator>(std::move(record_iterator));
                }

                return record_iterator;
            }

        private:

            inline auto get_file_size(const std::filesystem::path& file_path) -> size_t
            {
                std::ifstream file(file_path, std::ios::binary | std::ios::ate);
            
                if (!file.is_open())
                {
                    throw std::runtime_error("failed to open file");
                }

                return file.tellg();
            }

            class LastDelimiterExtendedMemorySpan: public virtual memory::ReadableMemorySpanInterface
            {
                private:

                    std::shared_ptr<memory::ReadableMemorySpanInterface> base;
                    char delimiter;

                public:

                    inline LastDelimiterExtendedMemorySpan(std::shared_ptr<memory::ReadableMemorySpanInterface> base,
                                                           char delimiter) noexcept: base(std::move(base)),
                                                                                     delimiter(delimiter){}

                    inline auto read(size_t offset, size_t sz) -> std::string
                    {
                        std::string base_rs     = this->base->read(offset, sz);
                        size_t delimiter_idx    = this->base->size(); 
                        size_t first            = offset;
                        size_t last             = offset + sz;

                        if (first <= delimiter_idx && last > delimiter_idx)
                        {
                            base_rs.push_back(this->delimiter);
                        }

                        return base_rs;
                    }

                    inline auto size() -> size_t
                    {
                        return this->base->size() + 1u;
                    } 
            };
    };
}

#endif