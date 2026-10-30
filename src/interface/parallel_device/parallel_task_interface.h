#ifndef __PARALLEL_DEVICE_PARALLEL_TASK_INTERFACE_H__
#define __PARALLEL_DEVICE_PARALLEL_TASK_INTERFACE_H__

namespace parallel_device
{
    class ParallelTaskInterface
    {
        public:

            virtual ~ParallelTaskInterface() noexcept = default;

            virtual void execute() noexcept = 0;
    };
}

#endif