#ifndef __SEMAPHORE_JOB_SEMAPHORE_H__
#define __SEMAPHORE_JOB_SEMAPHORE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <atomic>
#include <exception>
#include <stdexcept>
#include <semaphore>

namespace semaphore
{
    //because all the transactions that are associated with JobSemaphore are relaxed
    //thus it makes sense if we only use relax for this operation
    //because the otherwise requires an acquire and a release for a proper concurrent memory transaction

    class JobSemaphore
    {
        private:

            std::unique_ptr<std::counting_semaphore<>> job_smp;
            std::unique_ptr<std::atomic<uint32_t>> in_progress_counter;
            size_t job_counter; 

        public:

            inline JobSemaphore(size_t job_counter)
            {
                if (job_counter == 0u)
                {
                    throw std::invalid_argument("bad job counter, 0");
                }

                this->job_smp               = std::make_unique<std::counting_semaphore<>>(job_counter);
                this->in_progress_counter   = std::make_unique<std::atomic<uint32_t>>(0u);
                this->job_counter           = job_counter;
            }

            inline void acquire()
            {
                this->in_progress_counter->fetch_add(1u, std::memory_order_relaxed);
                this->job_smp->acquire();
            }

            inline void release()
            {
                this->job_smp->release();
                size_t old = this->in_progress_counter->fetch_sub(1u, std::memory_order_relaxed);
                
                if (old == 1u)
                {
                    this->in_progress_counter->notify_all();
                }
            }

            inline void wait_complete()
            {
                while (true)
                {
                    uint32_t in_progress_sz = this->in_progress_counter->load(std::memory_order_relaxed);

                    if (in_progress_sz == 0u)
                    {
                        return;
                    }

                    this->in_progress_counter->wait(1u, std::memory_order_relaxed);
                }
            } 

            inline auto is_completed() const noexcept -> bool
            {
                return this->in_progress_counter->load(std::memory_order_relaxed) == 0u;
            }

            inline auto job_count() const noexcept -> size_t
            {
                return this->job_counter;
            }
    };
}

#endif