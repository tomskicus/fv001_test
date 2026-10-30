#ifndef __PARALLEL_DEVICE_WORK_ORDER_QUEUE_H__
#define __PARALLEL_DEVICE_WORK_ORDER_QUEUE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <implementation/queue/bounded_queue.h>
#include <interface/parallel_device/parallel_task_interface.h> 
#include <interface/parallel_device/work_order_queue_interface.h>
#include <exception>
#include <optional>
#include <utility>
#include <stdexcept>

namespace parallel_device
{
    class FiniteWorkOrderQueue: public virtual WorkOrderQueueInterface
    {
        private:

            concurrent_queue::BoundedQueue<std::shared_ptr<ParallelTaskInterface>> queue;

        public:

            inline FiniteWorkOrderQueue(size_t cap): queue(cap){}

            inline void push(const std::shared_ptr<ParallelTaskInterface>& task)
            {
                if (task == nullptr)
                {
                    throw std::invalid_argument("bad task, null");
                }

                auto tmp_task   = task;
                this->queue.push(std::move(tmp_task));
            }

            inline auto pop() noexcept -> std::shared_ptr<ParallelTaskInterface>
            {
                std::optional<std::shared_ptr<ParallelTaskInterface>> rs    = this->queue.pop();

                if (!rs.has_value())
                {
                    return nullptr;
                }

                if (rs.value() == nullptr)
                {
                    std::abort();
                }

                return std::move(rs.value());
            }

            inline void poison() noexcept
            {
                this->queue.poison();
            }
    };
}

#endif