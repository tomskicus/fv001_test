#ifndef __LINUX_FILE_FILE_OPENER_INTERFACE_H__
#define __LINUX_FILE_FILE_OPENER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <fcntl.h>
#include <unistd.h>

namespace linux_file
{
    class FileOpenerInterface
    {
        public:

            virtual ~FileOpenerInterface() noexcept = default;

            virtual auto open_file(std::string_view file_path,
                                   int flags,
                                   mode_t mode) -> std::shared_ptr<int> = 0;
    };
}

#endif