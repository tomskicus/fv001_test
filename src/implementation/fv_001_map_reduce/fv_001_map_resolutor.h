#ifndef __FV_001_MAP_REDUCE_FV_001_MAP_RESOLUTOR_H__
#define __FV_001_MAP_REDUCE_FV_001_MAP_RESOLUTOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <interface/job_map_reduce/job_map_resolutor_interface.h>
#include <model/fv_001_records.h>
#include <model/fv_001_accumulatable.h>
#include <model/csv_work_order.h>
#include <interface/csv_reader/record_iterator_interface.h>
#include <implementation/csv_work_order/csv_work_order_as_record_iterator_factory.h> 

namespace fv_001_map_reduce
{
    class FV001MapResolutor: public virtual job_map_reduce::JobMapResolutorInterface
    {
        public:

            inline auto map(const model::CSVWorkOrder& work_order) -> std::shared_ptr<void>
            {
                std::unique_ptr<csv_reader::RecordIteratorInterface> record_iterator    = csv_work_order::CsvWorkOrderAsRecordIteratorFactory().to_record_iterator(work_order);
                model::FV001Accumulatable accumulatable                                 = this->accumulate(record_iterator);
                std::shared_ptr<model::FV001Accumulatable> result                       = std::make_shared<model::FV001Accumulatable>(accumulatable);

                return result;
            }

        private:

            //^^^
            //AI-generated, need generic resolution revolving reflection

            static inline constexpr size_t FV001_FIELD_COUNT = 6;

            inline uint64_t parse_u64(std::string_view token, const char* field_name)
            {
                uint64_t value = 0;
                const char* const first = token.data();
                const char* const last  = token.data() + token.size();

                const auto [ptr, ec] = std::from_chars(first, last, value);
                if (token.empty() || ec != std::errc{} || ptr != last)
                    throw std::runtime_error(std::string("FV001: invalid uint64 for '") + field_name +
                                            "': '" + std::string(token) + "<eor>");
                return value;
            }

            inline float parse_float(std::string_view token, const char* field_name)
            {
                float value = 0.0f;
                const char* const first = token.data();
                const char* const last  = token.data() + token.size();

                const auto [ptr, ec] = std::from_chars(first, last, value);
                if (token.empty() || ec != std::errc{} || ptr != last)
                    throw std::runtime_error(std::string("FV001: invalid float for '") + field_name +
                                            "': '" + std::string(token) + "'");
                return value;
            }

            inline bool is_digit(char c) { return c >= '0' && c <= '9'; }

            inline bool is_leap_year(int year)
            {
                return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
            }

            // Strict YYYY-MM-DD: exactly 10 chars, digits and '-' in the right places,
            // and a real calendar date.
            inline void validate_date(std::string_view d)
            {
                const auto fail = [&]() {
                    throw std::runtime_error("FV001: invalid date for 'date' (expected YYYY-MM-DD): '" +
                                            std::string(d) + "'");
                };

                if (d.size() != 10 || d[4] != '-' || d[7] != '-')
                    fail();

                for (size_t i : {0u, 1u, 2u, 3u, 5u, 6u, 8u, 9u})
                    if (!is_digit(d[i]))
                        fail();

                const int year  = (d[0] - '0') * 1000 + (d[1] - '0') * 100 + (d[2] - '0') * 10 + (d[3] - '0');
                const int month = (d[5] - '0') * 10 + (d[6] - '0');
                const int day   = (d[8] - '0') * 10 + (d[9] - '0');

                if (month < 1 || month > 12)
                    fail();

                static constexpr int days_in_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                int max_day = days_in_month[month - 1];
                if (month == 2 && is_leap_year(year))
                    max_day = 29;

                if (day < 1 || day > max_day)
                    fail();
            }

            inline auto deserialize_record(const std::string& line) -> model::FV001Record
            {
                // Strict split on ',' only. No trimming, no quote handling.
                std::array<std::string_view, FV001_FIELD_COUNT> tokens{};
                const std::string_view sv(line);

                size_t count = 0;
                size_t start = 0;
                while (true)
                {
                    const size_t comma = sv.find(',', start);
                    if (count == FV001_FIELD_COUNT)
                        throw std::runtime_error("FV001: too many fields (expected 6)");

                    if (comma == std::string_view::npos)
                    {
                        tokens[count++] = sv.substr(start);
                        break;
                    }
                    tokens[count++] = sv.substr(start, comma - start);
                    start = comma + 1;
                }

                if (count != FV001_FIELD_COUNT)
                    throw std::runtime_error("FV001: expected 6 fields, got " + std::to_string(count));

                validate_date(tokens[1]);

                model::FV001Record record{};
                record.campaign_id = std::string(tokens[0]);
                record.date        = std::string(tokens[1]);
                record.impressions = parse_u64(tokens[2], "impressions");
                record.clicks      = parse_u64(tokens[3], "clicks");
                record.spend       = parse_float(tokens[4], "spend");
                record.conversions = parse_u64(tokens[5], "conversions");
                return record;
            }

            //vvv

            inline auto accumulate(const std::unique_ptr<csv_reader::RecordIteratorInterface>& str_record_it) -> model::FV001Accumulatable
            {
                model::FV001Accumulatable accumulatable{};

                for (auto str_record = str_record_it->next(); str_record.has_value(); str_record = str_record_it->next())
                {
                    model::FV001Record record           = deserialize_record(str_record.value());

                    auto it = accumulatable.accumulatable_map.find(record.campaign_id);

                    if (it == accumulatable.accumulatable_map.end())
                    {
                        model::FV001AccumulatableValue value;

                        value.impressions  = record.impressions;
                        value.clicks       = record.clicks;
                        value.spend        = record.spend;
                        value.conversions  = record.conversions;

                        accumulatable.accumulatable_map.insert({record.campaign_id, value});
                    }
                    else
                    {
                        it->second.impressions += record.impressions;
                        it->second.clicks      += record.clicks;
                        it->second.spend       += record.spend;
                        it->second.conversions += record.conversions;
                    }
                }

                return accumulatable;
            }
    };
}

#endif