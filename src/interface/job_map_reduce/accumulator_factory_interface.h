#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_FACTORY_INTERFACE_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/job_map_reduce/job_reduce_resolutor_interface.h>
#include <interface/job_map_reduce/accumulator_interface.h>
#include <memory>

namespace job_map_reduce
{
    class AccumulatorFactoryInterface
    {
        public:

            virtual ~AccumulatorFactoryInterface() noexcept = default;

            virtual auto from_reduce_resolutor(const std::shared_ptr<JobReduceResolutorInterface>&) -> std::unique_ptr<AccumulatorInterface> = 0;
    };
}

#endif