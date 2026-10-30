#ifndef __CSV_WORK_ORDER_CSV_WORK_ORDER_FACTORY_H__
#define __CSV_WORK_ORDER_CSV_WORK_ORDER_FACTORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <implementation/csv_planner/file_segment_planner.h>
#include <optional>
#include <interface/csv_work_order/csv_work_order_factory_interface.h>
#include <utility>

namespace csv_work_order
{
    class CsvWorkOrderFactory: public virtual CsvWorkOrderFactoryInterface
    {
        private:

            std::optional<std::filesystem::path> file_path;

            char delimiter;
            bool skip_header;
            std::unique_ptr<csv_planner::FileSegmentPlannerInterface> segment_planner;

            static inline constexpr char DEFAULT_DELIMITER          = '\n';
            static inline constexpr bool DEFAULT_SKIP_HEADER        = true;

        public:

            inline CsvWorkOrderFactory(): file_path(std::nullopt),
                                          delimiter(DEFAULT_DELIMITER),
                                          skip_header(DEFAULT_SKIP_HEADER),
                                          segment_planner(std::make_unique<csv_planner::FileSegmentPlanner>()){}

            inline void set_file_path(const std::filesystem::path& file_path)
            {
                this->segment_planner->set_file_path(file_path);
                this->file_path = file_path;
            }

            inline void set_byte_chunk_size(size_t sz)
            {
                this->segment_planner->set_byte_chunk_size(sz);
            }

            inline void set_delimiter(char c)
            {
                this->delimiter     = c;
            }

            inline void set_skip_header(bool bit)
            {
                this->skip_header   = bit;
            }

            inline auto get() -> std::unique_ptr<CsvWorkOrderIteratorInterface>
            {
                if (!this->file_path.has_value())
                {
                    throw std::invalid_argument("bad file path, null");
                }

                if (this->get_file_size(this->file_path.value()) == 0u)
                {
                    return std::make_unique<InternalEmptyIterator>
                    (
                        this->file_path.value(),
                        this->delimiter,
                        this->skip_header
                    );
                }

                return std::make_unique<InternalIterator>
                (
                    this->file_path.value(),
                    this->delimiter,
                    this->skip_header,
                    this->segment_planner->get()
                );
            }
        
        private:
            
            inline auto get_file_size(const std::filesystem::path& file_path) -> size_t
            {
                std::ifstream file(file_path, std::ios::binary | std::ios::ate);
            
                if (!file.is_open())
                {
                    throw std::runtime_error("failed to open file");
                }

                return file.tellg();
            }

            class InternalIterator: public virtual CsvWorkOrderIteratorInterface
            {
                private:

                    std::filesystem::path file_path;
                    char delimiter;
                    bool skip_header;

                    std::unique_ptr<csv_planner::SegmentIteratorInterface> segment_iterator;

                public:

                    inline InternalIterator(std::filesystem::path file_path,
                                            char delimiter,
                                            bool skip_header,
                                            std::unique_ptr<csv_planner::SegmentIteratorInterface> segment_iterator): file_path(std::move(file_path)),
                                                                                                                      delimiter(delimiter),
                                                                                                                      skip_header(skip_header),
                                                                                                                      segment_iterator(std::move(segment_iterator)){}

                    inline auto next() -> std::optional<model::CSVWorkOrder>
                    {
                        std::optional<std::pair<size_t, size_t>> segment    = this->segment_iterator->next();

                        if (!segment.has_value())
                        {
                            return std::nullopt;
                        }

                        return model::CSVWorkOrder
                        {
                            .file_path      = this->file_path.native(),
                            .read_off       = segment->first,
                            .read_sz        = segment->second,
                            .skip_header    = this->skip_header,
                            .delimiter      = this->delimiter
                        };
                    }
            };

            class InternalEmptyIterator: public virtual CsvWorkOrderIteratorInterface
            {
                private:

                    std::filesystem::path file_path;
                    char delimiter;
                    bool skip_header;
                    bool is_completed;

                public:

                    inline InternalEmptyIterator(std::filesystem::path file_path,
                                                 char delimiter,
                                                 bool skip_header): file_path(file_path),
                                                                    delimiter(delimiter),
                                                                    skip_header(skip_header), 
                                                                    is_completed(false){}

                    inline auto next() -> std::optional<model::CSVWorkOrder>
                    {
                        if (std::exchange(this->is_completed, true))
                        {
                            return std::nullopt;
                        }

                        return model::CSVWorkOrder
                        {
                            .file_path      = this->file_path.native(),
                            .read_off       = size_t{0u},
                            .read_sz        = size_t{0u},
                            .skip_header    = this->skip_header,
                            .delimiter      = this->delimiter
                        };
                    }
            };
    };
}

#endif