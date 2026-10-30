#ifndef __CSV_PLANNER_SEGMENT_PLANNER_INTERFACE_H__
#define __CSV_PLANNER_SEGMENT_PLANNER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include "segment_iterator_interface.h"

namespace csv_planner
{
    class SegmentPlannerInterface
    {
        public:

            virtual ~SegmentPlannerInterface() noexcept = default;

            virtual void set_file_size(size_t) = 0;
            virtual void set_byte_chunk_size(size_t) = 0; 

            virtual auto get() -> std::unique_ptr<SegmentIteratorInterface> = 0;
    };
}
#endif