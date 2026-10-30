#ifndef __SERVICE_BUS_BUS_SENDER_CLIENT_INTERFACE_H__
#define __SERVICE_BUS_BUS_SENDER_CLIENT_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>

namespace service_bus
{
    class BusSenderClientInterface
    {
        public:

            virtual ~BusSenderClientInterface() noexcept = default;

            virtual void send(std::string_view) = 0;
    };
}
#endif