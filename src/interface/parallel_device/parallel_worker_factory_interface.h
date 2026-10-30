#ifndef __PARALLEL_DEVICE_PARALLEL_WORKER_INTERFACE_H__
#define __PARALLEL_DEVICE_PARALLEL_WORKER_INTERFACE_H__

namespace parallel_device
{
    class ParallelWorkerFactoryInterface
    {
        public:

            virtual ~ParallelWorkerFactoryInterface() noexcept = default;

            virtual auto get(const std::shared_ptr<WorkOrderQueueInterface>&) -> std::unique_ptr<concurrent_worker::ConcurrentWorkerInterface> = 0;
    };
}

#endif