#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_GROUP_MANAGER_FACTORY_INTERFACE_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_GROUP_MANAGER_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "job_reduce_resolutor_interface.h"
#include "accumulator_group_manager_interface.h"

namespace job_map_reduce
{
    class AccumulatorGroupManagerFactoryInterface //too generic
    {
        public:

            virtual ~AccumulatorGroupManagerFactoryInterface() noexcept = default;

            virtual void set_reduce_resolutor(const std::shared_ptr<JobReduceResolutorInterface>&) = 0;
            virtual void set_accumulator_size(size_t) = 0;

            virtual auto get() -> std::unique_ptr<AccumulatorGroupManagerInterface> = 0;
    };
}

#endif