#ifndef __SERVICE_BUS_BUS_RECEIVER_CLIENT_FACTORY_INTERFACE_H__
#define __SERVICE_BUS_BUS_RECEIVER_CLIENT_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <memory>
#include "bus_receiver_client_interface.h"

namespace service_bus
{
    class BusReceiverClientFactoryInterface
    {
        public:

            virtual ~BusReceiverClientFactoryInterface() noexcept = default;

            virtual auto create_receiver_client(std::string_view bus_id) -> std::shared_ptr<BusReceiverClientInterface> = 0;
    };
}

#endif