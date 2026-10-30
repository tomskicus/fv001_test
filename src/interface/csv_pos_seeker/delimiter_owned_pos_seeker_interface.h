#ifndef __CSV_POS_SEEKER_DELIMITER_OWNED_POS_SEEKER_INTERFACE_H__
#define __CSV_POS_SEEKER_DELIMITER_OWNED_POS_SEEKER_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <optional>
#include <vector>
#include <interface/memory/readable_memory_span_interface.h>

namespace csv_pos_seeker
{
    class DelimiterOwnedPosSeekerInterface
    {
        public:

            virtual ~DelimiterOwnedPosSeekerInterface() noexcept = default;

            virtual void set_memory_span(const std::shared_ptr<memory::ReadableMemorySpanInterface>&) = 0;
            virtual void set_record_delimiter(char c) = 0;

            virtual void set_offset(size_t) = 0;
            virtual void set_size(size_t) = 0;

            virtual auto get_owned_segment() -> std::optional<std::pair<size_t, size_t>> = 0;
    };
}

#endif