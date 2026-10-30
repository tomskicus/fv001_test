#ifndef __JOB_MAP_REDUCE_JOB_REDUCE_RESOLUTOR_INTERFACE_H__
#define __JOB_MAP_REDUCE_JOB_REDUCE_RESOLUTOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <memory>

namespace job_map_reduce
{
    class JobReduceResolutorInterface
    {
        public:

            virtual ~JobReduceResolutorInterface() noexcept = default;

            virtual auto reduce(const std::shared_ptr<void>&,
                                const std::shared_ptr<void>&) -> std::shared_ptr<void> = 0;
    };
}

#endif