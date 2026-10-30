#ifndef __SERVICE_BUS_BUS_RESOURCE_CONTROLLER_INTERFACE_H__
#define __SERVICE_BUS_BUS_RESOURCE_CONTROLLER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <memory>
#include "bus_queue_interface.h"
#include "bus_worker_interface.h"

namespace service_bus
{
    struct BusResource
    {
        std::shared_ptr<BusQueueInterface> queue;
        std::shared_ptr<BusWorkerInterface> worker;
    };

    class BusResourceControllerInterface
    {
        public:

            virtual ~BusResourceControllerInterface() noexcept = default;

            virtual void open_bus(std::string_view bus_id) = 0;
            virtual auto get_bus_resource(std::string_view bus_id) -> std::shared_ptr<BusResource> = 0;
            virtual void close_bus(std::string_view bus_id) noexcept = 0;
    };
}
#endif