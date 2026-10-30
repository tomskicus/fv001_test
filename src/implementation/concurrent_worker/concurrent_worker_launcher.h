#ifndef __CONCURRENT_WORKER_CONCURRENT_WORKER_LAUNCHER_H__
#define __CONCURRENT_WORKER_CONCURRENT_WORKER_LAUNCHER_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <exception>
#include <stdexcept>
#include <interface/concurrent_worker/concurrent_worker_interface.h>
#include <interface/concurrent_worker/concurrent_worker_launcher_interface.h>
#include <thread>

namespace concurrent_worker
{
    class ConcurrentWorkerLauncher: public virtual ConcurrentWorkerLauncherInterface
    {
        public:

            inline auto launch(const std::shared_ptr<ConcurrentWorkerInterface>& worker) -> std::shared_ptr<void>
            {
                if (worker == nullptr)
                {
                    throw std::invalid_argument("bad worker, null");
                }

                auto lambda = [=]() noexcept
                {
                    worker->start();
                };

                auto joiner = [=](std::thread * thr) noexcept
                {
                    worker->signal_interrupt();
                    thr->join();

                    delete thr;
                };

                std::unique_ptr<std::thread, decltype(joiner)> thr
                (
                    new std::thread(lambda),
                    joiner
                );

                return thr;
            }
    };
}

#endif