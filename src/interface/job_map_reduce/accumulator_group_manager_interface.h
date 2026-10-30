#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_GROUP_MANAGER_INTERFACE_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_GROUP_MANAGER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "accumulator_interface.h"

namespace job_map_reduce
{
    class AccumulatorGroupManagerInterface
    {
        public:

            virtual ~AccumulatorGroupManagerInterface() noexcept = default;

            virtual auto get() -> std::shared_ptr<AccumulatorInterface> = 0;
            virtual auto one() -> std::shared_ptr<void> = 0;
    };
}

#endif