#ifndef __JOB_MAP_REDUCE_JOB_TOPIC_INTERFACE_H__
#define __JOB_MAP_REDUCE_JOB_TOPIC_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <optional>
#include <model/csv_work_order.h>
#include <memory>

namespace job_map_reduce
{
    class JobTopicInterface
    {
        public:

            virtual ~JobTopicInterface() noexcept = default;

            virtual void submit_payload(const model::CSVWorkOrder&) = 0;
            virtual auto get() -> std::shared_ptr<void> = 0;
    };
}

#endif