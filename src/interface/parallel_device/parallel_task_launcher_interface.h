#ifndef __PARALLEL_DEVICE_PARALLEL_TASK_LAUNCHER_INTERFACE_H__
#define __PARALLEL_DEVICE_PARALLEL_TASK_LAUNCHER_INTERFACE_H__

#include "parallel_task_interface.h"
#include <memory>

namespace parallel_device
{
    class ParallelTaskLauncherInterface
    {
        public:

            virtual ~ParallelTaskLauncherInterface() noexcept = default;

            virtual void launch(const std::shared_ptr<ParallelTaskInterface>&) = 0;
    };
}

#endif