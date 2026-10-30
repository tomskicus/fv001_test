#ifndef __CSV_PLANNER_FILE_SEGMENT_PLANNER_H__
#define __CSV_PLANNER_FILE_SEGMENT_PLANNER_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/csv_planner/file_segment_planner_interface.h>
#include "segment_planner.h"
#include <fstream>
#include <stdexcept>
#include <exception>

namespace csv_planner
{
    class FileSegmentPlanner: public virtual FileSegmentPlannerInterface
    {
        private:

            std::optional<std::filesystem::path> file_path;
            std::unique_ptr<SegmentPlannerInterface> segment_planner;
        
        public:

            inline FileSegmentPlanner(): file_path(std::nullopt),
                                         segment_planner(std::make_unique<SegmentPlanner>()){}

            inline void set_file_path(const std::filesystem::path& file_path)
            {
                this->file_path = file_path;
            }

            inline void set_byte_chunk_size(size_t sz)
            {
                this->segment_planner->set_byte_chunk_size(sz);
            }

            inline auto get() -> std::unique_ptr<SegmentIteratorInterface>
            {
                this->segment_planner->set_file_size(this->get_file_size());

                return this->segment_planner->get();
            }

        private:

            inline auto get_file_size() -> size_t
            {
                if (!this->file_path.has_value())
                {
                    throw std::invalid_argument("bad file path, null");
                }

                std::ifstream file(this->file_path.value(),
                                   std::ios::binary | std::ios::ate);
            
                if (!file.is_open())
                {
                    throw std::runtime_error("failed to open file");
                }

                return file.tellg();
            }
    };
}

#endif