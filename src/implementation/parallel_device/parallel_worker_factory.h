#ifndef __PARALLEL_DEVICE_PARALLEL_WORKER_FACTORY_H__
#define __PARALLEL_DEVICE_PARALLEL_WORKER_FACTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "parallel_worker.h"
#include <interface/parallel_device/parallel_worker_factory_interface.h>

namespace parallel_device
{
    class ParallelWorkerFactory: public virtual ParallelWorkerFactoryInterface
    {
        public:

            inline auto get(const std::shared_ptr<WorkOrderQueueInterface>& queue) -> std::unique_ptr<concurrent_worker::ConcurrentWorkerInterface>
            {
                return std::make_unique<ParallelWorker>(queue);
            }
    };
}

#endif