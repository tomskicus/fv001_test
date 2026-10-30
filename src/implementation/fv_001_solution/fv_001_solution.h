#ifndef __FV_001_SOLUTION_FV_001_SOLUTION_H__
#define __FV_001_SOLUTION_FV_001_SOLUTION_H__

#include <stdint.h>
#include <stdlib.h>
#include <optional>
#include <filesystem>
#include <implementation/csv_work_order/csv_work_order_factory.h>
#include <implementation/job_map_reduce/job_map_reduce_executor.h>
#include <implementation/fv_001_map_reduce/fv_001_map_resolutor.h>
#include <implementation/fv_001_map_reduce/fv_001_reduce_resolutor.h>
#include <exception>
#include <stdexcept>

namespace fv_001_solution
{
    class FV001Solution
    {
        private:

            std::optional<std::filesystem::path> file_path;

            size_t map_parallel_job_sz;
            size_t reduce_parallel_job_sz;
            size_t tentative_read_unit_process_sz;

            static inline constexpr size_t DEFAULT_MAP_PARALLEL_JOB_SZ              = 1;
            static inline constexpr size_t DEFAULT_REDUCE_PARALLEL_JOB_SZ           = 1;
            static inline constexpr size_t DEFAULT_TENTATIVE_READ_UNIT_PROCESS_SZ   = size_t{1} << 12; 

        public:

            inline FV001Solution(): file_path(std::nullopt),
                                    map_parallel_job_sz(DEFAULT_MAP_PARALLEL_JOB_SZ),
                                    reduce_parallel_job_sz(DEFAULT_REDUCE_PARALLEL_JOB_SZ),
                                    tentative_read_unit_process_sz(DEFAULT_TENTATIVE_READ_UNIT_PROCESS_SZ){}

            inline void set_file_path(const std::filesystem::path& file_path)
            {
                this->file_path                         = file_path;
            }

            inline void set_map_parallel_job_size(size_t sz)
            {
                this->map_parallel_job_sz               = sz;
            }

            inline void set_reduce_parallel_job_size(size_t sz)
            {
                this->reduce_parallel_job_sz            = sz;
            }

            inline void set_tentative_read_unit_process_size(size_t sz)
            {
                this->tentative_read_unit_process_sz    = sz;
            }

            inline auto resolve() -> model::FV001Accumulatable
            {
                if (!this->file_path.has_value())
                {
                    throw std::invalid_argument("bad file path, null");
                }

                auto wo_factory             = csv_work_order::CsvWorkOrderFactory();
                auto map_reduce_executor    = job_map_reduce::JobMapReduceExecutor();

                wo_factory.set_file_path(this->file_path.value());
                wo_factory.set_byte_chunk_size(this->tentative_read_unit_process_sz);
                wo_factory.set_delimiter('\n');
                wo_factory.set_skip_header(true);

                std::unique_ptr<csv_work_order::CsvWorkOrderIteratorInterface> wo_iter  = wo_factory.get();

                map_reduce_executor.set_map_resolutor(std::make_unique<fv_001_map_reduce::FV001MapResolutor>());
                map_reduce_executor.set_reduce_resolutor(std::make_unique<fv_001_map_reduce::FV001ReduceResolutor>());
                map_reduce_executor.set_concurrent_map_size(this->map_parallel_job_sz);
                map_reduce_executor.set_concurrent_reduce_size(this->reduce_parallel_job_sz);

                std::unique_ptr<job_map_reduce::JobTopicInterface> job_topic    = map_reduce_executor.get();

                for (auto wo = wo_iter->next(); wo.has_value(); wo = wo_iter->next())
                {
                    job_topic->submit_payload(wo.value());
                }

                std::shared_ptr<void> result   = job_topic->get();

                if (result == nullptr)
                {
                    return model::FV001Accumulatable{};
                }

                return *static_pointer_cast<model::FV001Accumulatable>(result);
            }
    };
} 

#endif