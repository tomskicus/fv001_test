#ifndef __CSV_PLANNER_SEGMENT_PLANNER_H__
#define __CSV_PLANNER_SEGMENT_PLANNER_H__

#include <stdint.h>
#include <stdlib.h>
#include <optional>
#include <memory>
#include <interface/csv_planner/segment_planner_interface.h>
#include <interface/csv_planner/segment_iterator_interface.h> 

namespace csv_planner
{
    class SegmentPlanner: public virtual SegmentPlannerInterface
    {
        private:

            static inline constexpr size_t DEFAULT_BYTE_CHUNK_SZ    = size_t{1} << 12;

            std::optional<size_t> file_sz;
            size_t byte_chunk_sz;

        public:

            inline SegmentPlanner(): file_sz(std::nullopt),
                                     byte_chunk_sz(DEFAULT_BYTE_CHUNK_SZ){}

            inline void set_file_size(size_t sz)
            {
                this->file_sz       = sz;
            }

            inline void set_byte_chunk_size(size_t sz)
            {
                this->byte_chunk_sz = sz;
            }

            inline auto get() -> std::unique_ptr<SegmentIteratorInterface>
            {
                if (!this->file_sz.has_value())
                {
                    throw std::invalid_argument("bad file size, not set");
                }

                if (this->byte_chunk_sz == 0u)
                {
                    throw std::invalid_argument("bad byte chunk size, 0");
                }

                return std::make_unique<InternalSegmentIterator>(this->file_sz.value(),
                                                                 this->byte_chunk_sz);
            }
        
        private:
            
            class InternalSegmentIterator: public virtual SegmentIteratorInterface
            {
                private:

                    size_t first;
                    size_t last;
                    size_t increment_sz;
                
                public:

                    inline InternalSegmentIterator(size_t file_sz,
                                                   size_t chunk_sz): first(0u),
                                                                     last(file_sz),
                                                                     increment_sz(chunk_sz){}

                    inline auto next() -> std::optional<std::pair<size_t, size_t>>
                    {
                        if (this->first == this->last)
                        {
                            return std::nullopt;
                        }

                        size_t next_first                   = std::min(this->first + this->increment_sz,
                                                                       this->last);

                        std::pair<size_t, size_t> result    = std::make_pair
                        (
                            this->first,
                            next_first - this->first
                        );

                        this->first                         = next_first;

                        return result;
                    }
            };
    };
}

#endif