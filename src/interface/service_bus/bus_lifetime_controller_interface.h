#ifndef __SERVICE_BUS_BUS_LIFETIME_CONTROLLER_INTERFACE_H__
#define __SERVICE_BUS_BUS_LIFETIME_CONTROLLER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>

namespace service_bus
{
    class BusLifetimeControllerInterface
    {
        public:

            virtual ~BusLifetimeControllerInterface() noexcept = default;

            virtual void open_bus(std::string_view bus_id) = 0;
            virtual void close_bus(std::string_view bus_id) noexcept = 0;
    };
}

#endif