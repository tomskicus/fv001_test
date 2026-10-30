#ifndef __PARALLEL_DEVICE_PARALLEL_WORKER_H__
#define __PARALLEL_DEVICE_PARALLEL_WORKER_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <atomic>
#include <chrono>
#include <interface/concurrent_worker/concurrent_worker_interface.h> 
#include <interface/parallel_device/work_order_queue_interface.h>
#include <exception>
#include <stdexcept>
#include <thread>

namespace parallel_device
{
    class ParallelWorker: public virtual concurrent_worker::ConcurrentWorkerInterface
    {
        private:

            std::shared_ptr<WorkOrderQueueInterface> work_order_queue;
            std::atomic<bool> poison_pill;
            std::atomic<bool> start_guard; 

            static inline const std::chrono::milliseconds GRACE_PERIOD  = std::chrono::milliseconds(1); 

        public:

            inline ParallelWorker(std::shared_ptr<WorkOrderQueueInterface> work_order_queue)
            {
                if (work_order_queue == nullptr)
                {
                    throw std::invalid_argument("bad work order queue, null");
                }

                this->work_order_queue  = std::move(work_order_queue);
                this->poison_pill       = false;
                this->start_guard       = false;
            }

            inline void start()
            {
                bool old    = this->start_guard.exchange(true, std::memory_order_relaxed);

                if (old == true)
                {
                    throw std::invalid_argument("bad start, second start");
                }

                while (true)
                {
                    if (this->poison_pill.load(std::memory_order_relaxed))
                    {
                        break;
                    }

                    std::shared_ptr<ParallelTaskInterface> parallel_task    = this->work_order_queue->pop();

                    if (parallel_task == nullptr)
                    {
                        std::this_thread::sleep_for(GRACE_PERIOD);
                        continue;
                    }

                    parallel_task->execute();
                }
            }

            inline void signal_interrupt() noexcept
            {
                this->poison_pill.exchange(true, std::memory_order_relaxed);
            }
    };
}

#endif