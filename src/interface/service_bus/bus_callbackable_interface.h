#ifndef __SERVICE_BUS_BUS_CALLBACKABLE_INTERFACE_H__
#define __SERVICE_BUS_BUS_CALLBACKABLE_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>

namespace service_bus
{
    class BusCallbackableInterface
    {
        public:

            virtual ~BusCallbackableInterface() noexcept = default;

            virtual void callback(std::string_view msg) = 0;
    };
}
#endif