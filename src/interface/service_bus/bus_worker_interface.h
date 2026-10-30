#ifndef __SERVICE_BUS_BUS_WORKER_INTERFACE_H__
#define __SERVICE_BUS_BUS_WORKER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <memory>
#include "bus_callbackable_interface.h"
#include <interface/concurrent_worker/concurrent_worker_interface.h>

namespace service_bus
{
    class BusWorkerInterface: public virtual concurrent_worker::ConcurrentWorkerInterface
    {
        public:

            virtual ~BusWorkerInterface() noexcept = default;

            virtual void set_producer(const std::shared_ptr<BusQueueInterface>&) = 0; 

            virtual void insert_consumer(const std::shared_ptr<BusCallbackableInterface>&) = 0; 
            virtual void remove_consumer(const std::shared_ptr<BusCallbackableInterface>&) noexcept = 0;
    };
}
#endif