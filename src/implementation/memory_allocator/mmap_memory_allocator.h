#ifndef __MEMORY_ALLOCATOR_MMAP_MEMORY_ALLOCATOR_H__
#define __MEMORY_ALLOCATOR_MMAP_MEMORY_ALLOCATOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <cstring>
#include <interface/memory_allocator/memory_allocator_interface.h>

namespace memory_allocator
{
    class MmapMemoryAllocator: public virtual MemoryAllocatorInterface
    {
        private:

            struct MmapResource
            {
                std::shared_ptr<TempFile> tmp_file;
                std::shared_ptr<int> fd;
                size_t allocated_sz;
            };

            std::shared_ptr<TempFileGeneratorInterface> temp_file_generator;
            std::shared_ptr<linux_file::FileOpenerInterface> file_opener;
            std::unordered_map<uintptr_t, MmapResource> resource_map;
        
        public:

            inline MmapMemoryAllocator(): temp_file_generator(std::make_unique<file_utility::TempFileGenerator>()),
                                          file_opener(std::make_unique<linux_file::FileOpener>()),
                                          resource_map(){}

            inline auto malloc(size_t sz) -> void *
            {
                if (sz == 0u)
                {
                    return nullptr;
                }

                std::shared_ptr<TempFile> tmp_file  = this->temp_file_generator->get_temp_file();
                std::shared_ptr<int> fd             = this->file_opener->open_file(std::string_view(tmp_file.file_name()),
                                                                                   O_RDWR,
                                                                                   DEFAULT_FILE_OPEN_MODE);

                if (ftruncate(*fd, sz) == -1)
                {
                    throw std::bad_alloc();
                }

                void * mem                          = mmap
                (
                    nullptr,
                    sz,
                    PROT_READ | PROT_WRITE,
                    MAP_SHARED,
                    *fd,
                    0
                );

                if (mem == nullptr || mem == MAP_FAILED)
                {
                    throw std::bad_alloc();
                }

                try
                {
                    auto [_, status]    = this->resource_map.insert
                    (
                        std::make_pair
                        (
                            reinterpret_cast<uintptr_t>(mem),
                            MmapResource
                            {
                                .tmp_file       = tmp_file,
                                .fd             = fd,
                                .allocated_sz   = sz
                            }
                        )
                    );

                    if (status == false)
                    {
                        std::abort();
                    }
                }
                catch (...)
                {
                    std::abort();
                }

                return mem;
            }

            inline void free(void * ptr) noexcept
            {
                if (ptr == nullptr)
                {
                    return;
                }

                auto it = this->resource_map.find(reinterpret_cast<uintptr_t>(ptr));

                if (it == this->resource_map.end())
                {
                    std::abort();
                }

                munmap(ptr, it->second.allocated_sz);
                this->resource_map.erase(it);
            }
    };
} 

#endif