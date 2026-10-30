#ifndef __JOB_MAP_REDUCE_JOB_MAP_RESOLUTOR_INTERFACE_H__
#define __JOB_MAP_REDUCE_JOB_MAP_RESOLUTOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <memory>
#include <model/csv_work_order.h>

namespace job_map_reduce
{
    class JobMapResolutorInterface
    {
        public:

            virtual ~JobMapResolutorInterface() noexcept = default;

            virtual auto map(const model::CSVWorkOrder& csv_work_order) -> std::shared_ptr<void> = 0;
    };
}

#endif