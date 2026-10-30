#ifndef __BUS_SENDER_CLIENT_FACTORY_INTERFACE_H__
#define __BUS_SENDER_CLIENT_FACTORY_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <memory>
#include "bus_sender_client_interface.h"

namespace service_bus
{
    class BusSenderClientFactoryInterface
    {
        public:

            virtual ~BusSenderClientFactoryInterface() noexcept = default;

            virtual auto create_sender_client(std::string_view bus_id) -> std::shared_ptr<BusSenderClientInterface> = 0;
    };
}
#endif