#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_INTERFACE_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>

namespace job_map_reduce
{
    class AccumulatorInterface
    {
        public:

            virtual ~AccumulatorInterface() noexcept = default;

            virtual void accumulate(const std::shared_ptr<void>&) = 0;
            virtual auto get() -> std::shared_ptr<void> = 0;
    };
}

#endif