#ifndef __PARALLEL_DEVICE_PARALLEL_TASK_LAUNCHER_H__
#define __PARALLEL_DEVICE_PARALLEL_TASK_LAUNCHER_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <interface/parallel_device/parallel_task_launcher_interface.h>
#include <interface/parallel_device/work_order_queue_interface.h>

namespace parallel_device
{
    class ParallelTaskLauncher: public virtual ParallelTaskLauncherInterface
    {
        private:

            std::shared_ptr<WorkOrderQueueInterface> queue;
            std::shared_ptr<void> worker_resource;

        public:

            inline ParallelTaskLauncher(std::shared_ptr<WorkOrderQueueInterface> queue,
                                        std::shared_ptr<void> worker_resource) noexcept: queue(std::move(queue)),
                                                                                         worker_resource(std::move(worker_resource)){}

            inline ~ParallelTaskLauncher() noexcept
            {
                {
                    this->queue->poison();
                }
                {
                    this->worker_resource = nullptr;
                }
            }

            inline void launch(const std::shared_ptr<ParallelTaskInterface>& task)
            {
                this->queue->push(task);
            }
    };
}

#endif