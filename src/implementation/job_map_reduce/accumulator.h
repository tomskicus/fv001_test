#ifndef __JOB_MAP_REDUCE_ACCUMULATOR_H__
#define __JOB_MAP_REDUCE_ACCUMULATOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <interface/job_map_reduce/accumulator_interface.h>
#include <interface/job_map_reduce/job_reduce_resolutor_interface.h>
#include <mutex>
#include <stdexcept>
#include <exception>

namespace job_map_reduce
{
    class Accumulator: public virtual AccumulatorInterface
    {
        private:

            std::shared_ptr<job_map_reduce::JobReduceResolutorInterface> reduce_resolutor;
            std::shared_ptr<void> value;
            std::unique_ptr<std::mutex> mtx;

        public:

            inline Accumulator(const std::shared_ptr<job_map_reduce::JobReduceResolutorInterface>& reduce_resolutor)
            {
                if (reduce_resolutor == nullptr)
                {
                    throw std::invalid_argument("bad reduce resolutor, null");
                }

                this->reduce_resolutor  = reduce_resolutor;
                this->value             = nullptr;
                this->mtx               = std::make_unique<std::mutex>();
            }

            inline void accumulate(const std::shared_ptr<void>& value)
            {
                if (value == nullptr)
                {
                    throw std::invalid_argument("bad accumulating value, null");
                }

                std::lock_guard<std::mutex> lock(*this->mtx);

                if (this->value == nullptr)
                {
                    this->value = value;
                    return;
                }

                this->value = this->reduce_resolutor->reduce(this->value, value);
            }

            inline auto get() -> std::shared_ptr<void>
            {
                std::lock_guard<std::mutex> lock(*this->mtx);

                return this->value;
            }
    };
}

#endif