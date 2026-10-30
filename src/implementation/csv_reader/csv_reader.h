#ifndef __CSV_READER_CSV_READER_H__
#define __CSV_READER_CSV_READER_H__

#include <stdint.h>
#include <stdlib.h>
#include <filesystem>
#include <memory>
#include <vector>
#include <optional>
#include <interface/csv_reader/csv_reader_interface.h>
#include <interface/csv_reader/record_iterator_interface.h>
#include <interface/file_reader/direct_file_reader_interface.h>
#include <implementation/file_reader/linux_direct_file_reader.h>
#include <exception>
#include <stdexcept>
#include <deque>
#include <string>

namespace csv_reader
{
    class CsvReader: public virtual CsvReaderInterface
    {
        private:

            std::unique_ptr<file_reader::DirectFileReaderInterface> file_reader;
            std::optional<std::filesystem::path> file_path;
            std::optional<size_t> read_pos;
            std::optional<size_t> read_sz;
            char delimiter;

            static inline constexpr char DEFAULT_DELIMITER  = ',';
            static inline constexpr size_t MEMORY_ITER_SZ   = size_t{1} << 12;

        public:

            inline CsvReader(): file_reader(std::make_unique<file_reader::LinuxDirectFileReader>()),
                                file_path(std::nullopt),
                                read_pos(std::nullopt),
                                read_sz(std::nullopt),
                                delimiter(DEFAULT_DELIMITER){}

            inline void set_file_path(const std::filesystem::path& file_path)
            {
                this->file_path = file_path;
            }

            inline void set_read_position(std::optional<size_t> offset)
            {
                this->read_pos  = offset;
            }

            inline void set_read_size(std::optional<size_t> sz)
            {
                this->read_sz   = sz;
            }

            inline void set_delimiter(char c)
            {
                this->delimiter = c;
            }

            inline auto get_record_iterator() -> std::unique_ptr<RecordIteratorInterface>
            {
                if (!this->file_path.has_value())
                {
                    throw std::invalid_argument("bad file path, not set");
                }

                std::unique_ptr<memory::MemoryIteratorInterface> memory_iterator    = this->file_reader->read
                (
                    this->file_path.value(),
                    this->read_pos,
                    this->read_sz
                );

                return std::make_unique<InternalRecordIterator>
                (
                    std::move(memory_iterator),
                    this->MEMORY_ITER_SZ,
                    this->delimiter
                );
            }

        private:
            
            class InternalRecordIterator: public virtual RecordIteratorInterface
            {
                private:

                    std::unique_ptr<memory::MemoryIteratorInterface> memory_iterator;
                    size_t iter_sz;
                    char delimiter;
                    std::deque<std::string> record_vec;  
                    std::deque<char> leftover_record;
                    bool is_completed;

                public:

                    inline InternalRecordIterator(std::unique_ptr<memory::MemoryIteratorInterface> memory_iterator,
                                                  size_t iter_sz,
                                                  char delimiter): memory_iterator(std::move(memory_iterator)),
                                                                   iter_sz(iter_sz),
                                                                   delimiter(delimiter),
                                                                   record_vec(),
                                                                   leftover_record(),
                                                                   is_completed(false){}

                    inline auto next() -> std::optional<std::string>
                    {
                        if (this->record_vec.empty())
                        {
                            this->refill();
                        }

                        if (this->record_vec.empty())
                        {
                            return std::nullopt;
                        }

                        std::string rs  = std::move(this->record_vec.front());
                        this->record_vec.pop_front();

                        return rs;
                    }

                private:

                    inline void refill()
                    {
                        while (this->record_vec.empty())
                        {
                            std::optional<std::string> memory_segment   = this->memory_iterator->next(this->iter_sz);

                            if (!memory_segment.has_value())
                            {
                                if (is_completed)
                                {
                                    return;
                                }
                                else
                                {
                                    this->record_vec.push_back(std::string(this->leftover_record.begin(), this->leftover_record.end()));
                                    this->leftover_record.clear();
                                    this->is_completed  = true;

                                    return;
                                }
                            }
                            else
                            {
                                size_t i_first  = this->leftover_record.size();
                                size_t first    = 0u; 

                                this->leftover_record.insert
                                (
                                    this->leftover_record.end(),
                                    memory_segment->begin(),
                                    memory_segment->end()
                                );

                                for (size_t i = i_first; i < this->leftover_record.size(); ++i)
                                {
                                    if (this->leftover_record[i] == this->delimiter)
                                    {
                                        std::string record(std::next(this->leftover_record.begin(), first),
                                                           std::next(this->leftover_record.begin(), i));

                                        this->record_vec.push_back(std::move(record));
                                        first   = i + 1;
                                    }
                                }

                                this->leftover_record.erase
                                (
                                    this->leftover_record.begin(),
                                    std::next(this->leftover_record.begin(), first)
                                );
                            }
                        }
                    }
            };
    };
}

#endif