#ifndef __FILE_PARALLEL_READER_INTERFACE_H__
#define __FILE_PARALLEL_READER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>

namespace linux_file
{
    class FileParallelReaderInterface
    {
        public:

            virtual ~FileParallelReaderInterface() noexcept = default;

            virtual auto read(int fd,
                              void * dst,
                              size_t offset,
                              size_t sz) -> size_t = 0;
    };
}

#endif