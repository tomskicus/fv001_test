#ifndef __FINITE_PARALLEL_DEVICE_SINGLETON_H__
#define __FINITE_PARALLEL_DEVICE_SINGLETON_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <lib/stl_extension/stdx.h>
#include "finite_parallel_task_launcher_factory.h"

namespace parallel_device
{
    struct FiniteParallelDeviceSignature{};

    using SingletonObject   = stdx::singleton_container<std::shared_ptr<ParallelTaskLauncherInterface>,
                                                        FiniteParallelDeviceSignature>;

    inline void init(size_t concurrent_worker_sz,
                     size_t job_queue_capacity)
    {
        auto factory            = FiniteParallelTaskLauncherFactory();

        factory.set_concurrent_worker_size(concurrent_worker_sz);
        factory.set_job_queue_capacity(job_queue_capacity);

        SingletonObject::get()  = factory.get();
    }

    inline void deinit() noexcept
    {
        SingletonObject::get() = nullptr;
    }

    inline auto get_finite_parallel_device_singleton() noexcept -> const std::shared_ptr<ParallelTaskLauncherInterface>&
    {
        return SingletonObject::get();
    } 
}

#endif