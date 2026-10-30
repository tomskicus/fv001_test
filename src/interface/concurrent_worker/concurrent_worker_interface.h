#ifndef __CONCURRENT_WORKER_CONCURRENT_WORKER_INTERFACE_H__
#define __CONCURRENT_WORKER_CONCURRENT_WORKER_INTERFACE_H__

namespace concurrent_worker
{
    class ConcurrentWorkerInterface
    {
        public:

            virtual ~ConcurrentWorkerInterface() noexcept = default;

            virtual void start() = 0;
            virtual void signal_interrupt() noexcept = 0;
    };
}
#endif