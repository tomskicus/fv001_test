#ifndef __JOB_MAP_REDUCE_MAP_REDUCE_EXECUTOR_INTERFACE_H__
#define __JOB_MAP_REDUCE_MAP_REDUCE_EXECUTOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>

namespace job_map_reduce
{
    class JobMapReduceExecutorInterface
    {
        public:

            virtual ~JobMapReduceExecutorInterface() noexcept = default;

            virtual void set_map_resolutor(const std::shared_ptr<JobMapResolutorInterface>&) = 0;
            virtual void set_reduce_resolutor(const std::shared_ptr<JobReduceResolutorInterface>&) = 0;

            virtual void set_concurrent_map_size(size_t) = 0;
            virtual void set_concurrent_reduce_size(size_t) = 0;

            virtual auto get() -> std::unique_ptr<JobTopicInterface> = 0;
    };
}

#endif