#ifndef __PARALLEL_DEVICE_WORK_ORDER_QUEUE_INTERFACE_H__
#define __PARALLEL_DEVICE_WORK_ORDER_QUEUE_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "parallel_task_interface.h"

namespace parallel_device
{
    class WorkOrderQueueInterface
    {
        public:

            virtual ~WorkOrderQueueInterface() noexcept = default;

            virtual void push(const std::shared_ptr<ParallelTaskInterface>&) = 0;
            virtual auto pop() noexcept -> std::shared_ptr<ParallelTaskInterface> = 0;

            virtual void poison() noexcept = 0;
    };
}

#endif