#ifndef __LINUX_FILE_FILE_OPENER_H__
#define __LINUX_FILE_FILE_OPENER_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/linux_file/file_opener_interface.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory>
#include <exception/io_error.h>

namespace linux_file
{
    class FileOpener: public virtual FileOpenerInterface
    {
        public:

            inline auto open_file(std::string_view file_path,
                                  int flags,
                                  mode_t mode) -> std::shared_ptr<int>
            {
                std::string tmp(file_path);

                int fd  = open
                (
                    tmp.c_str(),
                    flags,
                    mode
                );

                if (fd == -1)
                {
                    throw exception::io_error();
                }

                auto destructor = [](int * fd_obj) noexcept
                {
                    close(*fd_obj);
                    delete fd_obj;
                };

                try
                {
                    return std::unique_ptr<int, decltype(destructor)>
                    (
                        new int{fd},
                        destructor
                    );
                }
                catch (...)
                {
                    std::abort();
                }
            }
    };
}

#endif