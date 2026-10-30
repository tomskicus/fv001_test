#ifndef __PARALLEL_DEVICE_PARALLEL_TASK_LAUNCHER_FACTORY_INTERFACE_H__
#define __PARALLEL_DEVICE_PARALLEL_TASK_LAUNCHER_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include "parallel_task_launcher_interface.h"

namespace parallel_device
{
    class FiniteParallelTaskLauncherFactoryInterface
    {
        public:

            virtual ~FiniteParallelTaskLauncherFactoryInterface() noexcept = default;

            virtual void set_concurrent_worker_size(size_t sz) = 0;
            virtual void set_job_queue_capacity(size_t) = 0;

            virtual auto get() -> std::unique_ptr<ParallelTaskLauncherInterface> = 0; 
    };
}

#endif