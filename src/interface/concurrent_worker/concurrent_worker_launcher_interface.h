#ifndef __CONCURRENT_WORKER_CONCURRENT_WORKER_LAUNCHER_INTERFACE_H__
#define __CONCURRENT_WORKER_CONCURRENT_WORKER_LAUNCHER_INTERFACE_H__

namespace concurrent_worker
{
    class ConcurrentWorkerLauncherInterface
    {
        public:

            virtual ~ConcurrentWorkerLauncherInterface() noexcept = default;

            virtual auto launch(const std::shared_ptr<ConcurrentWorkerInterface>& worker) -> std::shared_ptr<void> = 0;
    };
}

#endif