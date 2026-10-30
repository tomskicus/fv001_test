#ifndef __ACCUMULATOR_GROUP_MANAGER_FACTORY_H__
#define __ACCUMULATOR_GROUP_MANAGER_FACTORY_H__

#include <interface/job_map_reduce/accumulator_group_manager_factory_interface.h> 

#include "accumulator_factory.h"
#include "accumulator_group_manager.h"
#include <stdint.h>
#include <stdlib.h>
#include <memory>

namespace job_map_reduce
{
    class AccumulatorGroupManagerFactory: public virtual AccumulatorGroupManagerFactoryInterface
    {
        private:

            std::shared_ptr<JobReduceResolutorInterface> job_reduce_resolutor;
            size_t accumulator_sz;
            size_t concurrent_one_worker_sz;

            static inline constexpr size_t DEFAULT_ACCUMULATOR_SZ           = 8u;
            static inline constexpr size_t DEFAULT_CONCURRENT_ONE_WORKER_SZ = 8u;

        public:

            inline AccumulatorGroupManagerFactory(): job_reduce_resolutor(nullptr),
                                                     accumulator_sz(DEFAULT_ACCUMULATOR_SZ),
                                                     concurrent_one_worker_sz(DEFAULT_CONCURRENT_ONE_WORKER_SZ){}

            inline void set_reduce_resolutor(const std::shared_ptr<JobReduceResolutorInterface>& job_reduce_resolutor)
            {
                this->job_reduce_resolutor  = job_reduce_resolutor;
            }

            inline void set_accumulator_size(size_t sz)
            {
                this->accumulator_sz        = sz;
            }

            inline auto get() -> std::unique_ptr<AccumulatorGroupManagerInterface>
            {
                std::vector<std::shared_ptr<AccumulatorInterface>> accumulator_vec{};

                for (size_t i = 0u; i < this->accumulator_sz; ++i)
                {
                    accumulator_vec.push_back
                    (
                        AccumulatorFactory().from_reduce_resolutor
                        (
                            this->job_reduce_resolutor
                        )
                    );
                }

                return std::make_unique<AccumulatorGroupManager>
                (
                    accumulator_vec,
                    this->job_reduce_resolutor,
                    this->accumulator_sz
                );
            }
    };
}

#endif