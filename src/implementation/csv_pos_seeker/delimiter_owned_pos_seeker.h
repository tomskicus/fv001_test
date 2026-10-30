#ifndef __CSV_POS_SEEKER_DELIMITER_OWNED_POS_SEEKER_H__
#define __CSV_POS_SEEKER_DELIMITER_OWNED_POS_SEEKER_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/csv_pos_seeker/delimiter_owned_pos_seeker_interface.h>
#include <exception/corrupted_memory_access.h>

namespace csv_pos_seeker
{
    class DelimiterOwnedPosSeeker: public virtual DelimiterOwnedPosSeekerInterface
    {
        private:

            char record_delimiter;

            std::shared_ptr<memory::ReadableMemorySpanInterface> memory_span;

            std::optional<char> eor_delimiter;
            std::optional<size_t> usr_offset;
            std::optional<size_t> usr_sz;

            static inline constexpr char DEFAULT_RECORD_DELIMITER           = ',';
            static inline const std::optional<char> DEFAULT_EOR_DELIMITER   = '\0';

        public:

            inline DelimiterOwnedPosSeeker(): record_delimiter(DEFAULT_RECORD_DELIMITER),
                                              memory_span(nullptr),
                                              eor_delimiter(DEFAULT_EOR_DELIMITER),
                                              usr_offset(std::nullopt),
                                              usr_sz(std::nullopt){}

            inline void set_memory_span(const std::shared_ptr<memory::ReadableMemorySpanInterface>& span)
            {
                this->memory_span       = span;
            }

            inline void set_record_delimiter(char c)
            {
                this->record_delimiter  = c;
            }

            inline void set_offset(size_t offset)
            {
                this->usr_offset        = offset;
            }

            inline void set_size(size_t sz)
            {
                this->usr_sz            = sz;
            }

            inline auto get_owned_segment() -> std::optional<std::pair<size_t, size_t>>
            {
                std::optional<size_t> actual_last   = this->get_actual_last();

                if (!actual_last.has_value())
                {
                    return std::nullopt;
                }

                size_t actual_first                 = this->get_actual_first();

                return std::make_pair
                (
                    actual_first,
                    actual_last.value() - actual_first
                );
            }

        private:
            
            inline auto get_tentative_first_last() -> std::pair<size_t, size_t>
            {
                if (this->memory_span == nullptr)
                {
                    throw std::invalid_argument("bad memory span, null");
                }

                size_t tentative_first  = this->usr_offset.value_or(size_t{0u});
                size_t tentative_last;

                if (this->usr_sz.has_value())
                {
                    tentative_last  = tentative_first + this->usr_sz.value();
                }
                else
                {
                    tentative_last  = std::max(tentative_first, this->memory_span->size());
                }

                size_t fixed_first      = std::clamp
                (
                    tentative_first,
                    size_t{0u},
                    this->memory_span->size()
                );

                size_t fixed_last      = std::clamp
                (
                    tentative_last,
                    tentative_first,
                    this->memory_span->size()
                );

                return std::make_pair
                (
                    fixed_first,
                    fixed_last
                );
            }

            inline auto is_delimiter(char c) -> bool
            {
                if (c == this->record_delimiter)
                {
                    return true;
                }

                return false;
            }

            inline auto is_delimiter_at_index(size_t idx) -> bool
            {
                std::string str = this->memory_span->read(idx, 1u);

                if (str.size() != 1u)
                {
                    throw exception::corrupted_memory_access();
                }

                return this->is_delimiter(str[0]);
            }

            inline auto get_actual_last() -> std::optional<size_t>
            {
                if (this->memory_span == nullptr)
                {
                    throw std::invalid_argument("bad memory span, null");
                }

                auto [tentative_first, tentative_last]  = this->get_tentative_first_last();
                size_t iterable_sz                      = tentative_last - tentative_first; 

                //going through the range
                //truncate the last to the nearest delimiter (due to our owning logics of each delimiter being paired with the content preceding it)
                //return std::nullopt otherwise to indicate that there are no delimiters in the range 

                for (size_t i = 0u; i < iterable_sz; ++i)
                {
                    size_t back_idx = tentative_last - (i + 1u);

                    if (this->is_delimiter_at_index(back_idx))
                    {
                        return back_idx;
                    }
                }

                return std::nullopt;
            }

            inline auto get_actual_first() -> size_t
            {
                if (this->memory_span == nullptr)
                {
                    throw std::invalid_argument("bad memory span, null");
                }

                auto [tentative_first, _]   = this->get_tentative_first_last();
                size_t iterable_sz          = tentative_first;

                for (size_t i = 0u; i < iterable_sz; ++i)
                {
                    size_t back_idx = tentative_first - (i + 1u);

                    if (this->is_delimiter_at_index(back_idx))
                    {
                        return back_idx + 1u;
                    }
                }

                return 0u;
            }
    };
}

#endif