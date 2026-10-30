#ifndef __EXCEPTION_CORRUPTED_MEMORY_ACCESS_H__
#define __EXCEPTION_CORRUPTED_MEMORY_ACCESS_H__

#include <exception>
#include <stdexcept>

namespace exception
{
    struct corrupted_memory_access: std::runtime_error
    {
        inline corrupted_memory_access(): std::runtime_error("bad access, corrupted access"){}
    };
}

#endif