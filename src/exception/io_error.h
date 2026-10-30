#ifndef __EXCEPTION_IO_ERROR_H__
#define __EXCEPTION_IO_ERROR_H__

#include <exception>
#include <stdexcept>

namespace exception
{
    struct io_error: std::runtime_error
    {
        inline io_error(): std::runtime_error("IOError"){}
    };
}

#endif