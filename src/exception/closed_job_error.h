#ifndef __EXCEPTION_CLOSED_JOB_ERROR_H__
#define __EXCEPTION_CLOSED_JOB_ERROR_H__

#include <stdint.h>
#include <stdlib.h>
#include <exception>
#include <stdexcept>

namespace exception
{
    struct closed_job_error: std::invalid_argument
    {
        inline closed_job_error(): std::invalid_argument("bad operation, closed job"){}
    };
}

#endif