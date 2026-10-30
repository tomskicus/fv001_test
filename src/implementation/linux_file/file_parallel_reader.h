#ifndef __LINUX_FILE_FILE_PARALLEL_READER_H__
#define __LINUX_FILE_FILE_PARALLEL_READER_H__

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <exception/io_error.h>
#include <interface/linux_file/file_parallel_reader_interface.h>

namespace linux_file
{
    class FileParallelReader: public virtual FileParallelReaderInterface
    {
        public:

            inline auto read(int fd,
                             void * dst,
                             size_t offset,
                             size_t sz) -> size_t 
            {
                intmax_t rs = pread(fd,
                                    dst,
                                    sz,
                                    offset);

                if (rs < 0)
                {
                    throw exception::io_error();
                }

                return rs;
            }
    };
}

#endif