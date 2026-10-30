#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_GROUP_MANAGER_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_GROUP_MANAGER_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <implementation/thread_utility/thread_indexer.h>
#include <vector>
#include "parallel_reduce.h"
#include <interface/job_map_reduce/accumulator_interface.h>
#include <interface/job_map_reduce/job_reduce_resolutor_interface.h>
#include <interface/job_map_reduce/accumulator_group_manager_interface.h>

namespace job_map_reduce
{
    class AccumulatorGroupManager: public virtual AccumulatorGroupManagerInterface
    {
        private:

            std::vector<std::shared_ptr<AccumulatorInterface>> accumulator_vec;
            std::shared_ptr<JobReduceResolutorInterface> reduce_resolutor;
            size_t concurrent_one_reduce_sz;

        public:

            inline AccumulatorGroupManager(const std::vector<std::shared_ptr<AccumulatorInterface>>& accumulator_vec,
                                           const std::shared_ptr<JobReduceResolutorInterface>& reduce_resolutor,
                                           size_t concurrent_one_reduce_sz)
            {
                for (const auto& accumulator: accumulator_vec)
                {
                    if (accumulator == nullptr)
                    {
                        throw std::invalid_argument("bad accumulator, null");
                    }
                }

                if (accumulator_vec.size() == 0u)
                {
                    throw std::invalid_argument("bad accumulator vector size, 0");
                }

                if (reduce_resolutor == nullptr)
                {
                    throw std::invalid_argument("bad reduce resolutor, null");
                }

                if (concurrent_one_reduce_sz == 0u)
                {
                    throw std::invalid_argument("bad concurrent one reduce size, 0");
                }

                this->accumulator_vec           = accumulator_vec;
                this->reduce_resolutor          = reduce_resolutor;
                this->concurrent_one_reduce_sz  = concurrent_one_reduce_sz;
            }

            inline auto get() -> std::shared_ptr<AccumulatorInterface>
            {
                size_t idx  = this->get_bucket_idx();

                return this->accumulator_vec[idx];
            }

            inline auto one() -> std::shared_ptr<void>
            {
                std::vector<std::shared_ptr<void>> resource_vec{}; //if this is not immutable, race condition would be present, which is the case in our application
                                                                   //one and get is not race safe, it means that one() is called post accumulation as a safe guard guarantee, and not during
                                                                   //a lot of problems could be present calling this function, without calling it once, post accum phase
                                                                   //ranging from mutable std::shared_ptr<void> to race access to etc.
                                                                   //but that is what it is, we deal with true concurrency, and there are no way we'd want to put a lock upfront at the vector side, it is wrong

                for (const auto& accumulator: this->accumulator_vec)
                {
                    resource_vec.push_back(accumulator->get());
                }

                return parallel_reduce(resource_vec,
                                       this->reduce_resolutor,
                                       this->concurrent_one_reduce_sz);
            }

        private:

            inline auto get_bucket_idx() -> size_t
            {
                //lock contention, hold a perfect hashing policy locally to make sure that we are keeping best practices

                size_t thr_idx  = thread_utility::ThreadIndexer::get_index();
                size_t slot     = thr_idx % this->accumulator_vec.size();

                return slot;
            }
    };
}

#endif