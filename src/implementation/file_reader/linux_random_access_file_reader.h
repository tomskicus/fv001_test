#ifndef __FILE_READER_LINUX_RANDOM_ACCESS_FILE_READER_H__
#define __FILE_READER_LINUX_RANDOM_ACCESS_FILE_READER_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <interface/file_reader/file_as_memory_span_factory_interface.h>
#include <interface/memory/readable_memory_span_interface.h>
#include <implementation/linux_file/file_opener.h>
#include <implementation/linux_file/file_parallel_reader.h>
#include <fstream>
#include <filesystem>
#include <lib/stl_extension/datastructure.h>
#include <exception/corrupted_memory_access.h>
#include "assert.h"

namespace file_reader
{
    //I would still think virtual memory serves a better purpose
    //but it makes sense if we don't pollute the shared space of what should be kept and should not be
    //even though I would still try to make a memory allocation function and compromise the shared property instead of writing it like this
    //I feel like I'm reinventing what virtual memory should be like, and in the gray area of what should be on disk

    class LinuxRandomAccessFileReader: public virtual FileAsMemorySpanFactoryInterface
    {
        private:

            std::unique_ptr<linux_file::FileOpenerInterface> file_opener;

            static inline constexpr size_t POW2_REGION_SZ           = size_t{1} << 10;
            static inline constexpr size_t CACHE_MAP_CAP            = size_t{1} << 4;

            static inline constexpr mode_t DEFAULT_FILE_OPEN_MODE   = 0666;

        public:

            inline LinuxRandomAccessFileReader(): file_opener(std::make_unique<linux_file::FileOpener>()){}

            inline auto to_memory_span(const std::filesystem::path& file_path) -> std::shared_ptr<memory::ReadableMemorySpanInterface>
            {
                std::string_view file_path_str_view = std::string_view(file_path.native());
                std::shared_ptr<int> fd             = this->file_opener->open_file
                (
                    file_path_str_view,
                    O_RDONLY,
                    DEFAULT_FILE_OPEN_MODE
                );

                return std::make_unique<InternalMemorySpan>
                (
                    fd,
                    std::make_unique<linux_file::FileParallelReader>(),
                    CACHE_MAP_CAP,
                    POW2_REGION_SZ,
                    this->get_file_size(file_path)
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

            class InternalMemorySpan: public virtual memory::ReadableMemorySpanInterface
            {
                private:

                    std::shared_ptr<int> fd;
                    std::unique_ptr<linux_file::FileParallelReaderInterface> file_parallel_reader;
                    datastructure::unordered_map_variants::cyclic_unordered_node_map<size_t, std::string> cached_memory_region;
                    size_t pow2_region_sz;
                    size_t file_sz;

                public:

                    inline InternalMemorySpan(std::shared_ptr<int> fd,
                                              std::unique_ptr<linux_file::FileParallelReaderInterface> file_parallel_reader,
                                              size_t map_cap,
                                              size_t pow2_region_sz,
                                              size_t file_sz) noexcept: fd(std::move(fd)),
                                                                        file_parallel_reader(std::move(file_parallel_reader)),
                                                                        cached_memory_region(map_cap),
                                                                        pow2_region_sz(pow2_region_sz),
                                                                        file_sz(file_sz){}

                    inline auto read(size_t offset, size_t sz) -> std::string
                    {
                        offset              = std::min
                        (
                            offset,
                            this->file_sz
                        );

                        size_t max_sz       = this->file_sz - offset;
                        sz                  = std::min
                        (
                            sz,
                            max_sz
                        );

                        std::string rs  = {};

                        for (size_t i = 0u; i < sz; ++i)
                        {
                            size_t mem_ptr          = offset + i;
                            size_t region_offset    = mem_ptr & (this->pow2_region_sz - 1u);
                            size_t region           = mem_ptr - region_offset;
                            auto it                 = this->cached_memory_region.find(region);

                            if (it != this->cached_memory_region.end()) [[likely]]
                            {
                                rs.push_back(it->second[region_offset]);
                            }
                            else [[unlikely]]
                            {
                                rs.push_back(this->slow_fetch_char_at(mem_ptr));
                            }
                        }

                        return rs;
                    }

                    inline auto size() -> size_t
                    {
                        return file_sz;
                    }

                private:

                    inline __attribute__((noinline)) auto slow_fetch_char_at(size_t mem_ptr) -> char
                    {
                        if (mem_ptr >= this->file_sz)
                        {
                            std::abort();
                        }

                        size_t region_offset    = mem_ptr & (this->pow2_region_sz - 1u);
                        size_t region           = mem_ptr - region_offset;
                        auto it                 = this->cached_memory_region.find(region);

                        if (it == this->cached_memory_region.end())
                        {
                            size_t nxt_region   = region + this->pow2_region_sz;
                            nxt_region          = std::min
                            (
                                nxt_region,
                                this->file_sz
                            );
                            size_t bsz          = nxt_region - region;

                            std::string mem(bsz, ' ');

                            size_t actual_sz    = this->file_parallel_reader->read
                            (
                                *this->fd,
                                mem.data(),
                                region,
                                nxt_region - region
                            );

                            if (actual_sz != bsz)
                            {
                                throw exception::corrupted_memory_access();
                            }

                            auto [new_it, status]   = this->cached_memory_region.insert
                            (
                                std::make_pair
                                (
                                    region,
                                    std::move(mem)
                                )
                            );

                            assert(status);
                            it = new_it;
                        }

                        return it->second[region_offset];
                    }
            };
        
    };
}

#endif