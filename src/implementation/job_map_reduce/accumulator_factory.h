#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_FACTORY_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_FACTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "accumulator.h"
#include <interface/job_map_reduce/accumulator_factory_interface.h>

namespace job_map_reduce
{
    class AccumulatorFactory: public virtual AccumulatorFactoryInterface
    {
        public:

            inline auto from_reduce_resolutor(const std::shared_ptr<JobReduceResolutorInterface>& reduce_resolutor) -> std::unique_ptr<AccumulatorInterface>
            {
                return std::make_unique<Accumulator>(reduce_resolutor);
            }            
    };
} 

#endif