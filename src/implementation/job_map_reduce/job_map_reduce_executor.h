#ifndef __JOB_MAP_REDUCE_JOB_MAP_REDUCE_EXECUTOR_H__
#define __JOB_MAP_REDUCE_JOB_MAP_REDUCE_EXECUTOR_H__

#include <stdint.h>
#include <stdlib.h>
#include "map_reduce_job_topic.h"
#include <implementation/parallel_device/finite_parallel_device_singleton.h>
#include <interface/job_map_reduce/job_map_reduce_executor_interface.h>

namespace job_map_reduce
{
    class JobMapReduceExecutor: public virtual JobMapReduceExecutorInterface
    {
        private:

            std::shared_ptr<JobMapResolutorInterface> map_resolutor;
            std::shared_ptr<JobReduceResolutorInterface> reduce_resolutor;

            size_t concurrent_map_sz;
            size_t concurrent_reduce_sz;

            static inline constexpr size_t DEFAULT_CONCURRENT_MAP_SZ    = 1u;
            static inline constexpr size_t DEFAULT_CONCURRENT_REDUCE_SZ = 1u;

        public:

            inline JobMapReduceExecutor(): map_resolutor(nullptr),
                                           reduce_resolutor(nullptr),
                                           concurrent_map_sz(DEFAULT_CONCURRENT_MAP_SZ),
                                           concurrent_reduce_sz(DEFAULT_CONCURRENT_REDUCE_SZ){}

            inline void set_map_resolutor(const std::shared_ptr<JobMapResolutorInterface>& map_resolutor)
            {
                this->map_resolutor         = map_resolutor;
            }

            inline void set_reduce_resolutor(const std::shared_ptr<JobReduceResolutorInterface>& reduce_resolutor)
            {
                this->reduce_resolutor      = reduce_resolutor;
            }

            inline void set_concurrent_map_size(size_t sz)
            {
                this->concurrent_map_sz     = sz;
            }

            inline void set_concurrent_reduce_size(size_t sz)
            {
                this->concurrent_reduce_sz  = sz;
            }

            inline auto get() -> std::unique_ptr<JobTopicInterface>
            {
                return std::make_unique<MapReduceJobTopic>
                (
                    parallel_device::get_finite_parallel_device_singleton(),
                    this->map_resolutor,
                    this->reduce_resolutor,
                    this->concurrent_map_sz,
                    this->concurrent_reduce_sz
                );
            }
    };
}

#endif