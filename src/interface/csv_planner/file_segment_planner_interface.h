#ifndef __CSV_SEGMENT_PLANNER_INTERFACE_H__
#define __CSV_SEGMENT_PLANNER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <filesystem>
#include <memory>
#include "segment_iterator_interface.h"

namespace csv_planner
{
    class FileSegmentPlannerInterface
    {
        public:

            virtual ~FileSegmentPlannerInterface() noexcept = default;

            virtual void set_file_path(const std::filesystem::path&) = 0;
            virtual void set_byte_chunk_size(size_t) = 0; 

            virtual auto get() -> std::unique_ptr<SegmentIteratorInterface> = 0;
    };
}

#endif