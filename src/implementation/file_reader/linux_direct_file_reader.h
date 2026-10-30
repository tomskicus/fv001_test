#ifndef __FILE_READER_LINUX_DIRECT_FILE_READER_H__
#define __FILE_READER_LINUX_DIRECT_FILE_READER_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/file_reader/direct_file_reader_interface.h>
#include <implementation/linux_file/file_opener.h>
#include <implementation/linux_file/file_parallel_reader.h>
#include <fstream>
#include <filesystem>

namespace file_reader
{
    class LinuxDirectFileReader: public virtual DirectFileReaderInterface
    {
        private:

            std::unique_ptr<linux_file::FileOpenerInterface> file_opener;

            static inline constexpr mode_t DEFAULT_FILE_OPEN_MODE = 0666;

        public:

            inline LinuxDirectFileReader(): file_opener(std::make_unique<linux_file::FileOpener>()){}

            inline auto read(const std::filesystem::path& file_path,
                             std::optional<size_t> offset,
                             std::optional<size_t> sz) -> std::unique_ptr<memory::MemoryIteratorInterface>
            {
                size_t file_sz  = this->get_file_size(file_path); 

                if (!offset.has_value())
                {
                    offset = 0u;
                }
                else
                {
                    offset.value()  = std::min
                    (
                        offset.value(),
                        file_sz
                    );
                }

                size_t max_sz   = file_sz - offset.value(); 

                if (!sz.has_value())
                {
                    sz  = max_sz;
                }
                else
                {
                    sz.value()      = std::min
                    (
                        sz.value(),
                        max_sz
                    );
                }

                std::string_view file_path_str_view = std::string_view(file_path.native());
                std::shared_ptr<int> fd             = this->file_opener->open_file
                (
                    file_path_str_view,
                    O_RDONLY,
                    DEFAULT_FILE_OPEN_MODE
                );

                return std::make_unique<InternalMemoryIterator>
                (
                    fd,
                    std::make_unique<linux_file::FileParallelReader>(),
                    offset.value(),
                    offset.value() + sz.value()
                );
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

            class InternalMemoryIterator: public virtual memory::MemoryIteratorInterface
            {
                private:

                    std::shared_ptr<int> fd;
                    std::unique_ptr<linux_file::FileParallelReaderInterface> file_parallel_reader;
                    size_t byte_first;
                    size_t byte_last; 

                public:

                    inline InternalMemoryIterator(std::shared_ptr<int> fd,
                                                  std::unique_ptr<linux_file::FileParallelReaderInterface> file_parallel_reader,
                                                  size_t byte_first,
                                                  size_t byte_last) noexcept: fd(std::move(fd)),
                                                                              file_parallel_reader(std::move(file_parallel_reader)),
                                                                              byte_first(byte_first),
                                                                              byte_last(byte_last){}

                    inline auto next(size_t byte_sz) -> std::optional<std::string>
                    {
                        if (this->byte_first == this->byte_last)
                        {
                            return std::nullopt;
                        }

                        if (byte_sz == 0u)
                        {
                            return std::nullopt;
                        }

                        size_t next_byte_first      = std::min
                        (
                            this->byte_first + byte_sz,
                            this->byte_last
                        );

                        size_t tentative_read_sz    = next_byte_first - this->byte_first; 
                        std::string bstream(tentative_read_sz, ' ');
                        size_t actual_read_sz       = this->file_parallel_reader->read
                        (
                            *this->fd,
                            bstream.data(),
                            this->byte_first,
                            tentative_read_sz
                        );

                        bstream.resize(actual_read_sz);

                        if (actual_read_sz != tentative_read_sz)
                        {
                            this->byte_first    = this->byte_last;
                        }
                        else
                        {
                            this->byte_first    = next_byte_first;
                        }

                        return std::move(bstream);
                    }
            };
    };
}

#endif