#ifndef __FINITE_PARALLEL_TASK_LAUNCHER_FACTORY_H__
#define __FINITE_PARALLEL_TASK_LAUNCHER_FACTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <exception>
#include <stdexcept>
#include <interface/parallel_device/finite_parallel_task_launcher_factory_interface.h>
#include <interface/parallel_device/work_order_queue_interface.h> 
#include "finite_work_order_queue.h"
#include <implementation/concurrent_worker/concurrent_worker_launcher.h>
#include "parallel_worker_factory.h"
#include "parallel_task_launcher.h"

namespace parallel_device
{
    class FiniteParallelTaskLauncherFactory: public virtual FiniteParallelTaskLauncherFactoryInterface
    {
        private:

            size_t concurrent_worker_sz;
            size_t job_queue_capacity;

            static inline constexpr size_t DEFAULT_CONCURRENT_WORKER_SZ = 4u;
            static inline constexpr size_t DEFAULT_JOB_QUEUE_CAPACITY   = size_t{1} << 10;

        public:

            inline FiniteParallelTaskLauncherFactory(): concurrent_worker_sz(DEFAULT_CONCURRENT_WORKER_SZ),
                                                        job_queue_capacity(DEFAULT_JOB_QUEUE_CAPACITY){}

            inline void set_concurrent_worker_size(size_t sz)
            {
                if (sz == 0u)
                {
                    throw std::invalid_argument("bad concurrent worker size, 0");
                }

                this->concurrent_worker_sz  = concurrent_worker_sz;
            }

            inline void set_job_queue_capacity(size_t sz)
            {
                if (sz == 0u)
                {
                    throw std::invalid_argument("bad job queue capacity, 0");
                }

                this->job_queue_capacity    = sz;
            }

            inline auto get() -> std::unique_ptr<ParallelTaskLauncherInterface>
            {
                std::shared_ptr<WorkOrderQueueInterface> wo_queue   = this->get_finite_workorder_queue();
                std::shared_ptr<void> worker                        = this->get_worker_resource_handle(wo_queue);

                //critical sections, deadlocks on memory exhaustion

                try
                {
                    return std::make_unique<ParallelTaskLauncher>
                    (
                        std::move(wo_queue),
                        std::move(worker)
                    );
                }
                catch (...)
                {
                    std::abort();
                }
            }

        private:
            
            inline auto get_finite_workorder_queue() -> std::unique_ptr<WorkOrderQueueInterface>
            {
                return std::make_unique<FiniteWorkOrderQueue>
                (
                    this->job_queue_capacity
                );
            }

            inline auto get_worker_resource_handle(const std::shared_ptr<WorkOrderQueueInterface>& workorder_queue) -> std::shared_ptr<void>
            {
                std::vector<std::shared_ptr<void>> worker_vec{};

                for (size_t i = 0u; i < this->concurrent_worker_sz; ++i)
                {
                    std::unique_ptr<concurrent_worker::ConcurrentWorkerInterface> concurrent_worker = ParallelWorkerFactory().get(workorder_queue);
                    
                    try
                    {
                        std::shared_ptr<void> worker_handle = concurrent_worker::ConcurrentWorkerLauncher().launch(std::move(concurrent_worker));
                        worker_vec.push_back(std::move(worker_handle));
                    }
                    catch (...)
                    {
                        std::abort();
                    }
                }

                return std::make_unique<decltype(worker_vec)>(std::move(worker_vec));
            }
    };
}

#endif