#ifndef __JOB_MAP_REDUCE_MAP_REDUCE_JOB_TOPIC_H__
#define __JOB_MAP_REDUCE_MAP_REDUCE_JOB_TOPIC_H__

#include <interface/parallel_device/parallel_task_launcher_interface.h>
#include <interface/job_map_reduce/job_topic_interface.h>
#include <interface/job_map_reduce/job_map_resolutor_interface.h>
#include <interface/job_map_reduce/job_reduce_resolutor_interface.h>

#include <implementation/semaphore/job_semaphore.h>
#include <implementation/job_map_reduce/accumulator_group_manager_factory.h>

#include <exception/thread_safe_exception_holder.h>
#include <exception/closed_job_error.h>

#include <stdint.h>
#include <stdlib.h>
#include <memory>

#include <model/csv_work_order.h>

namespace job_map_reduce
{
    class MapReduceJobTopic: public virtual job_map_reduce::JobTopicInterface
    {
        private:

            std::shared_ptr<parallel_device::ParallelTaskLauncherInterface> parallel_device;
            std::shared_ptr<job_map_reduce::JobMapResolutorInterface> map_resolutor;

            std::shared_ptr<semaphore::JobSemaphore> job_semaphore;
            std::shared_ptr<job_map_reduce::AccumulatorGroupManagerInterface> distributed_accumulator;

            std::shared_ptr<exception::ThreadSafeExceptionHolder> map_exception;
            bool is_completed;

        public:

            inline MapReduceJobTopic(std::shared_ptr<parallel_device::ParallelTaskLauncherInterface> parallel_device,
                                     std::shared_ptr<job_map_reduce::JobMapResolutorInterface> map_resolutor,
                                     std::shared_ptr<job_map_reduce::JobReduceResolutorInterface> reduce_resolutor,
                                     size_t map_parallel_job_sz,
                                     size_t reduce_parallel_job_sz)
            {
                if (parallel_device == nullptr)
                {
                    throw std::invalid_argument("bad parallel device, null");
                }

                if (map_resolutor == nullptr)
                {
                    throw std::invalid_argument("bad map_resolutor, null");
                }

                if (reduce_resolutor == nullptr)
                {
                    throw std::invalid_argument("bad reduce resolutor, null");
                }

                if (map_parallel_job_sz == 0u)
                {
                    throw std::invalid_argument("bad map_parallel_job_sz, 0");
                }

                if (reduce_parallel_job_sz == 0u)
                {
                    throw std::invalid_argument("bad reduce_parallel_job_sz, 0");
                }

                this->parallel_device           = parallel_device;
                this->map_resolutor             = map_resolutor;
                this->job_semaphore             = std::make_shared<semaphore::JobSemaphore>(map_parallel_job_sz);

                {
                    auto factory                    = AccumulatorGroupManagerFactory();
                    
                    factory.set_reduce_resolutor(reduce_resolutor);
                    factory.set_accumulator_size(reduce_parallel_job_sz);

                    this->distributed_accumulator   = factory.get();
                }

                this->map_exception             = std::make_shared<exception::ThreadSafeExceptionHolder>();
                this->is_completed              = false;
            }

            inline void submit_payload(const model::CSVWorkOrder& csv_workorder)
            {
                if (this->is_completed)
                {
                    throw exception::closed_job_error();
                }

                std::unique_ptr<parallel_device::ParallelTaskInterface> parallel_job = std::make_unique<ParallelJob>
                (
                    this->map_resolutor,
                    this->job_semaphore,
                    this->distributed_accumulator,
                    this->map_exception,
                    csv_workorder
                );

                this->job_semaphore->acquire();

                try
                {
                    this->parallel_device->launch(std::move(parallel_job));
                }
                catch (...)
                {
                    this->job_semaphore->release();
                    throw;
                }
            }

            inline auto get() -> std::shared_ptr<void>
            {
                if (this->is_completed)
                {
                    throw exception::closed_job_error();
                }

                this->job_semaphore->wait_complete();

                this->is_completed = true;
                this->map_exception->throw_ex();

                return this->distributed_accumulator->one();
            }

        private:

            class ParallelJob: public virtual parallel_device::ParallelTaskInterface
            {
                private:

                    std::shared_ptr<job_map_reduce::JobMapResolutorInterface> map_resolutor;
                    std::shared_ptr<semaphore::JobSemaphore> job_semaphore;
                    std::shared_ptr<job_map_reduce::AccumulatorGroupManagerInterface> distributed_accumulator;
                    std::shared_ptr<exception::ThreadSafeExceptionHolder> map_exception;
                    model::CSVWorkOrder payload;

                public:

                    inline ParallelJob(std::shared_ptr<job_map_reduce::JobMapResolutorInterface> map_resolutor,
                                       std::shared_ptr<semaphore::JobSemaphore> job_semaphore,
                                       std::shared_ptr<job_map_reduce::AccumulatorGroupManagerInterface> distributed_accumulator,
                                       std::shared_ptr<exception::ThreadSafeExceptionHolder> map_exception,
                                       model::CSVWorkOrder payload) noexcept: map_resolutor(std::move(map_resolutor)),
                                                                              job_semaphore(std::move(job_semaphore)),
                                                                              distributed_accumulator(std::move(distributed_accumulator)),
                                                                              map_exception(std::move(map_exception)),
                                                                              payload(std::move(payload)){}

                    inline void execute() noexcept
                    {
                        try
                        {
                            std::shared_ptr<void> result = this->map_resolutor->map(this->payload);
                            std::shared_ptr<job_map_reduce::AccumulatorInterface> accumulator   = this->distributed_accumulator->get();
                            accumulator->accumulate(result);
                        }
                        catch (...)
                        {
                            this->map_exception->on_exception(std::current_exception());
                        }

                        this->job_semaphore->release();
                    }
            };
    };
}

#endif