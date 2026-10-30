#ifndef __SERVICE_BUS_BUS_RECEIVER_CLIENT_INTERFACE_H__
#define __SERVICE_BUS_BUS_RECEIVER_CLIENT_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "bus_callbackable_interface.h"

namespace service_bus
{
    class BusReceiverClientInterface
    {
        public:
        
            virtual ~BusReceiverClientInterface() noexcept = default;

            virtual void set_consumer(const std::shared_ptr<BusCallbackableInterface>&) = 0;
            virtual void set_concurrent_worker_size(size_t) = 0;

            virtual auto get_handle() -> std::shared_ptr<void> = 0;
    };
}

#endif