#ifndef __JOB_MAP_REDUCE_PARALLEL_REDUCER_H__
#define __JOB_MAP_REDUCE_PARALLEL_REDUCER_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <vector>
#include <deque>
#include <implementation/semaphore/job_semaphore.h>
#include <implementation/parallel_device/lambda_as_task.h>
#include <mutex>
#include <implementation/parallel_device/finite_parallel_device_singleton.h>
#include <interface/job_map_reduce/job_reduce_resolutor_interface.h>
#include <exception/thread_safe_exception_holder.h>

namespace job_map_reduce
{
    inline auto parallel_reduce(const std::vector<std::shared_ptr<void>>& accumulatable_vec,
                                const std::shared_ptr<JobReduceResolutorInterface>& reduce_resolutor,
                                size_t concurrent_reduce_sz) -> std::shared_ptr<void>
    {
        if (reduce_resolutor == nullptr)
        {
            throw std::invalid_argument("bad reduce resolutor, null");
        }

        if (concurrent_reduce_sz == 0u)
        {
            throw std::invalid_argument("bad concurrent reduce size, 0");
        }

        std::shared_ptr<std::deque<std::shared_ptr<void>>> accum_dq = std::make_shared<std::deque<std::shared_ptr<void>>>
        (
            std::deque<std::shared_ptr<void>>
            (
                accumulatable_vec.begin(),
                accumulatable_vec.end()
            )
        );

        const std::shared_ptr<parallel_device::ParallelTaskLauncherInterface>& parallel_device  = parallel_device::get_finite_parallel_device_singleton();

        std::shared_ptr<std::mutex> dq_mtx                              = std::make_shared<std::mutex>();
        std::shared_ptr<semaphore::JobSemaphore> smp                    = std::make_shared<semaphore::JobSemaphore>(concurrent_reduce_sz);
        std::shared_ptr<exception::ThreadSafeExceptionHolder> ex_holder = std::make_shared<exception::ThreadSafeExceptionHolder>();
        std::shared_ptr<void> result                                    = nullptr;

        while (true)
        {
            std::shared_ptr<void> item0 = nullptr;
            std::shared_ptr<void> item1 = nullptr;
            bool should_wait            = false;

            {
                std::lock_guard<std::mutex> lock(*dq_mtx);

                if (smp->is_completed())
                {
                    if (accum_dq->empty())
                    {
                        result  = nullptr;
                        break;
                    }

                    if (accum_dq->size() == 1u)
                    {
                        result  = accum_dq->front();
                        break;
                    }

                    item0   = accum_dq->at(0);
                    item1   = accum_dq->at(1);

                    accum_dq->pop_front();
                    accum_dq->pop_front();
                }
                else
                {
                    if (accum_dq->size() < 2)
                    {
                        should_wait = true;                                
                    }
                    else
                    {
                        item0   = accum_dq->at(0);
                        item1   = accum_dq->at(1);

                        accum_dq->pop_front();
                        accum_dq->pop_front();
                    }
                }
            }

            if (should_wait)
            {
                smp->wait_complete(); //should be wait next, bad unit
            }
            else
            {
                auto job    = [=]() noexcept
                {
                    try
                    {
                        if (item0 != nullptr || item1 != nullptr)
                        {
                            std::shared_ptr<void> item01;

                            if (item0 != nullptr && item1 != nullptr)
                            {
                                item01  = reduce_resolutor->reduce(item0, item1);
                            }
                            else if (item0 != nullptr)
                            {
                                item01  = item0;
                            }
                            else
                            {
                                item01  = item1;
                            }

                            {
                                std::lock_guard<std::mutex> lock(*dq_mtx);
                                accum_dq->push_back(item01);
                            }
                        }
                    }
                    catch (...)
                    {
                        ex_holder->on_exception(std::current_exception());
                    }

                    smp->release();
                };

                smp->acquire();

                parallel_device->launch
                (
                    parallel_device::lambda_as_task(job)
                );
            }
        }

        ex_holder->throw_ex();

        return result;
    }
}

#endif