#ifndef __SERVICE_BUS_BUS_QUEUE_INTERFACE_H__
#define __SERVICE_BUS_BUS_QUEUE_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>

namespace service_bus
{
    class BusQueueInterface
    {
        public:

            virtual ~BusQueueInterface() noexcept = default;

            virtual void push(std::string_view msg) = 0;
            virtual auto pop() noexcept -> std::string = 0;
            virtual void poison() noexcept = 0;
    };
}

#endif