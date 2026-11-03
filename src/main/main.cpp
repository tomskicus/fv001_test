#define DEBUG_MODE_FLAG true
#define STRONG_MEMORY_ORDERING_FLAG true

#include <implementation/fv_001_solution/fv_001_solution.h>
#include <filesystem>

struct Record
{
    std::string campaign_id;

    uint64_t impressions;
    uint64_t clicks;
    float spend;
    uint64_t conversions;

    float ctr;
    float cpa;
};

auto compare_float(float lhs, float rhs) -> int
{
    int is_lhs_nan  = std::isnan(lhs);
    int is_rhs_nan  = std::isnan(rhs);
    
    if (is_lhs_nan > is_rhs_nan)
    {
        return -1;
    }

    if (is_lhs_nan < is_rhs_nan)
    {
        return 1;
    }

    if (lhs < rhs)
    {
        return -1;
    }

    if (lhs > rhs)
    {
        return 1;
    }

    return 0;
}

auto top10_ctr(const std::vector<Record>& record_vec) -> std::vector<Record>
{
    auto less = [](const auto& lhs, const auto& rhs)
    {
        return compare_float(lhs.ctr, rhs.ctr) < 0;
    };

    auto tmp_record_vec = record_vec;

    std::make_heap
    (
        tmp_record_vec.begin(),
        tmp_record_vec.end(),
        less
    );

    std::vector<Record> rs{};

    for (size_t i = 0u; i < 10; ++i)
    {
        if (tmp_record_vec.empty())
        {
            break;
        }

        std::pop_heap
        (
            tmp_record_vec.begin(),
            tmp_record_vec.end(),
            less
        );

        rs.push_back
        (
            tmp_record_vec.back()
        );

        tmp_record_vec.pop_back();
    }

    return rs;
}

auto low10_cpa(const std::vector<Record>& record_vec) -> std::vector<Record>
{
    auto greater = [](const auto& lhs, const auto& rhs)
    {
        return compare_float(lhs.cpa, rhs.cpa) > 0;
    };

    auto tmp_record_vec = record_vec;

    std::make_heap
    (
        tmp_record_vec.begin(),
        tmp_record_vec.end(),
        greater
    );

    std::vector<Record> rs{};

    for (size_t i = 0u; i < 10; ++i)
    {
        if (tmp_record_vec.empty())
        {
            break;
        }

        std::pop_heap
        (
            tmp_record_vec.begin(),
            tmp_record_vec.end(),
            greater
        );

        rs.push_back
        (
            tmp_record_vec.back()
        );

        tmp_record_vec.pop_back();
    }

    return rs;
}

auto record_to_string(const Record& record) -> std::string
{
    return record.campaign_id + "," 
        + std::to_string(record.impressions) + ","
        + std::to_string(record.clicks) + ","
        + std::to_string(record.spend) + ","
        + std::to_string(record.conversions) + ","
        + std::to_string(record.ctr) + ","
        + std::to_string(record.cpa); 
}

auto join(const std::vector<std::string>& str_vec,
          char c) -> std::string
{
    if (str_vec.empty())
    {
        return {};
    }

    std::string rs = str_vec.front();

    for (size_t i = 1u; i < str_vec.size(); ++i)
    {
        rs.push_back(c);
        rs.insert
        (
            rs.end(),
            str_vec[i].begin(),
            str_vec[i].end()
        );
    }

    return rs;
}

auto get_header_string() -> std::string
{
    return "campaign_id,total_impressions,total_clicks,total_spend,total_conversions,CTR,CPA";
}

auto record_vec_to_writable(const std::vector<Record>& record_vec) -> std::string
{
    std::vector<std::string> record_str_vec{};

    record_str_vec.push_back(get_header_string());

    for (const auto& record: record_vec)
    {
        record_str_vec.push_back(record_to_string(record));
    }

    return join(record_str_vec, '\n');
} 

void write_file(const std::filesystem::path& file_path,
                const std::string& content)
{
    std::fstream fstream(file_path, std::ios::out | std::ios::binary);

    if (!fstream)
    {
        throw std::runtime_error("cannot open for writing: " + file_path.string());
    }

    fstream.write(content.data(), content.size());
}

void run(const std::filesystem::path& in_file_path,
         const std::filesystem::path& out_file_path,
         size_t parallel_worker_sz)
{
    parallel_device::init(parallel_worker_sz, size_t{1} << 8);

    std::filesystem::create_directories(out_file_path);   //

    size_t concurrent_map_worker_sz     = parallel_worker_sz;
    size_t concurrent_reduce_worker_sz  = parallel_worker_sz;
    size_t read_unit_sz                 = size_t{1} << 12;

    fv_001_solution::FV001Solution solution{};

    solution.set_file_path(in_file_path);
    solution.set_map_parallel_job_size(concurrent_map_worker_sz);
    solution.set_reduce_parallel_job_size(concurrent_reduce_worker_sz);
    solution.set_tentative_read_unit_process_size(read_unit_sz);

    model::FV001Accumulatable result    = solution.resolve();
    std::vector<Record> record_vec      = {};

    for (const auto& bucket: result.accumulatable_map)
    {
        record_vec.push_back
        (
            Record
            {
                .campaign_id    = bucket.first,
                .impressions    = bucket.second.impressions,
                .clicks         = bucket.second.clicks,
                .spend          = bucket.second.spend,
                .conversions    = bucket.second.conversions,
                .ctr            = static_cast<float>(bucket.second.clicks) / bucket.second.impressions,
                .cpa            = static_cast<float>(bucket.second.spend) / bucket.second.conversions
            }
        );
    }

    std::vector<Record> top10_ctr_rec   = top10_ctr(record_vec);
    std::vector<Record> low10_cpa_rec   = low10_cpa(record_vec);

    write_file
    (
        out_file_path / "top10_ctr.csv",
        record_vec_to_writable(top10_ctr_rec)
    );

    write_file
    (
        out_file_path / "top10_cpa.csv",
        record_vec_to_writable(low10_cpa_rec)
    );

    parallel_device::deinit();
}

int main(int argc, char ** argv)
{
    std::filesystem::path in_file_path  = {};
    std::filesystem::path out_file_path = "result";

    size_t parallel_worker_sz           = 8u;

    if (argc < 2)
    {
        std::cout << "please specify file path as the first argument";
        std::abort();
    }
    else
    {
        in_file_path        = std::string(argv[1]);
    }

    if (argc >= 3)
    {
        out_file_path       = std::string(argv[2]);
    }

    if (argc >= 4)
    {
        std::string arg     = std::string(argv[3]);
        uint32_t tmp;
        std::from_chars(arg.data(), std::next(arg.data(), arg.size()), tmp);

        parallel_worker_sz          = tmp;
    }

    run
    (
        in_file_path,
        out_file_path,
        parallel_worker_sz
    );

    return 0;
}
