#define STRONG_MEMORY_ORDERING_FLAG true
#define DEBUG_MODE_FLAG true

#include <implementation/csv_work_order/csv_work_order_as_record_iterator_factory.h>
#include <implementation/csv_work_order/csv_work_order_factory.h>
#include <implementation/csv_reader/csv_reader.h>
#include <random>
#include <algorithm>
#include <functional>
#include <chrono>
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>

auto randomize_int(size_t first, size_t last) -> size_t
{
    if (first == last)
    {
        throw std::invalid_argument("bad range, 0");
    }

    static auto randomizer  = std::bind(std::uniform_int_distribution<size_t>{},
                                        std::mt19937_64{static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())});

    
    size_t range    = last - first;
    size_t i        = randomizer() % range;

    return first + i;
}

auto randomize_bool() -> bool
{
    return randomize_int(0, 2) == 1;
}

auto randomize_az_AZ_09() -> char
{
    size_t az_range         = ('z' - 'a') + 1;
    size_t AZ_range         = ('Z' - 'A') + 1;
    size_t zero_nine_range  = ('9' - '0') + 1;

    size_t range            = az_range + AZ_range + zero_nine_range;
    size_t idx              = randomize_int(0u, range);

    if (idx < az_range)
    {
        return 'a' + idx;
    }

    if (idx < az_range + AZ_range)
    {
        return 'A' + (idx - az_range);
    }

    return '0' + (idx - az_range - AZ_range);
}

auto get_random_filename() -> std::string
{
    const size_t FILENAME_SZ    = 8u;
    std::string rs              = {};

    for (size_t i = 0u; i < FILENAME_SZ; ++i)
    {
        rs.push_back(randomize_az_AZ_09());
    }

    return rs;
}

auto get_random_record() -> std::string
{
    const size_t RECORD_SZ_RANGE    = 8u;
    size_t record_sz                = randomize_int(0u, RECORD_SZ_RANGE);
    std::string rs                  = {};

    for (size_t i = 0u; i < record_sz; ++i)
    {
        rs.push_back(randomize_az_AZ_09());
    }

    return rs;
}

auto get_random_record_list() -> std::vector<std::string>
{
    size_t SZ_EXP_RANGE     = 16u;
    size_t sz               = size_t{1} << randomize_int(0u, SZ_EXP_RANGE);

    std::vector<std::string> rs_vec{};

    for (size_t i = 0u; i < sz; ++i)
    {
        rs_vec.push_back(get_random_record());
    }

    return rs_vec;
}

auto get_csv_writable(const std::vector<std::string>& record_vec) -> std::string
{
    if (record_vec.empty())
    {
        return {};
    }

    std::string rs = record_vec[0];

    for (size_t i = 1u; i < record_vec.size(); ++i)
    {
        rs.push_back(',');
        rs.insert(rs.end(), record_vec[i].begin(), record_vec[i].end());
    }

    return rs;
}

auto get_test_file(const std::string& content) -> std::shared_ptr<std::filesystem::path>
{
    std::filesystem::path tmp_file_path(get_random_filename() + ".csv");

    std::fstream fstream(tmp_file_path, std::ios::out | std::ios::binary);
    
    try
    {
        fstream.write(content.data(), content.size());
    }
    catch (...)
    {
        try
        {
            std::filesystem::remove(tmp_file_path);
        }
        catch (...){}

        throw;
    }

    auto destructor = [](std::filesystem::path * fp)
    {
        std::filesystem::remove(*fp);
        delete fp;
    };

    return std::unique_ptr<std::filesystem::path, decltype(destructor)>
    (
        new std::filesystem::path(tmp_file_path),
        destructor
    );
}

auto parse_csv_content(const std::string& content) -> std::vector<std::string>
{
    std::vector<std::string> rs = {};
    size_t first                = 0u;

    for (size_t i = 0u; i < content.size(); ++i)
    {
        if (content[i] == ',')
        {
            rs.push_back
            (
                std::string
                (
                    std::next(content.begin(), first),
                    std::next(content.begin(), i)
                )
            );

            first   = i + 1;
        }
    }

    rs.push_back
    (
        std::string
        (
            std::next(content.begin(), first),
            content.end()
        )
    );

    return rs;
} 

auto get_test_interval(size_t sz) -> std::pair<size_t, size_t>
{
    size_t first    = randomize_int(0u, sz + 1);
    size_t rem      = sz - first;
    size_t last     = first + randomize_int(0u, rem + 1);

    return std::make_pair
    (
        first,
        last - first
    );
}

void print(const std::vector<std::string>& record_vec)
{
    std::cout << "record_count > " << record_vec.size() << "\n";

    for (const auto& record: record_vec)
    {
        std::cout << "record_size > " << record.size() << "\n";
        std::cout << "record > " << record << "\n"; 
    }
}

void run_one_test()
{
    const size_t INSPECTION_SZ                      = size_t{1} << 4;
    const size_t READ_SZ_RANGE                      = size_t{1} << 8;

    std::vector<std::string> record_vec             = get_random_record_list();
    std::string csv_writable                        = get_csv_writable(record_vec);
    std::shared_ptr<std::filesystem::path> tmp_file = get_test_file(csv_writable);

    size_t read_sz                                  = randomize_int(0u, READ_SZ_RANGE) + 1u; 
    bool skip_header                                = randomize_bool();
    
    if (record_vec.empty())
    {
        std::cout << "mayday, bad record vector size, 0\n";
        std::abort();
    }

    if (skip_header)
    {
        record_vec = std::vector<std::string>
        (
            std::next(record_vec.begin()),
            record_vec.end()
        );
    }

    auto wo_factory    = csv_work_order::CsvWorkOrderFactory();

    wo_factory.set_file_path(*tmp_file);
    wo_factory.set_delimiter(',');
    wo_factory.set_skip_header(skip_header);

    std::vector<model::CSVWorkOrder> csv_work_order_vec{};
    std::unique_ptr<csv_work_order::CsvWorkOrderIteratorInterface> wo_iterator  = wo_factory.get();

    for (auto it = wo_iterator->next(); it.has_value(); it = wo_iterator->next())
    {
        csv_work_order_vec.push_back(it.value());
    }

    std::vector<std::string> test_record_vec{};
    auto it_factory     = csv_work_order::CsvWorkOrderAsRecordIteratorFactory();

    for (const auto& work_order: csv_work_order_vec)
    {
        auto rc_iterator = it_factory.to_record_iterator(work_order);

        for (auto it = rc_iterator->next(); it.has_value(); it = rc_iterator->next())
        {
            test_record_vec.push_back(it.value());
        }
    }

    if (test_record_vec != record_vec)
    {
        std::cout << "bad test record vector, mismatched value\n";

        std::cout << "__TEST_RECORD_VEC__\n";
        print(test_record_vec);

        std::cout << "__RECORD_VEC__\n";
        print(record_vec);

        std::abort();
    }
}

void run_test()
{
    const size_t TEST_SZ    = size_t{1} << 16;
    const size_t COUT_SZ    = size_t{1} << 0;

    for (size_t i = 0u; i < TEST_SZ; ++i)
    {
        run_one_test();

        if (i % COUT_SZ == 0u)
        {
            std::cout << i << "/" << TEST_SZ << "\n";
        }
    }
}

int main()
{
    run_test();
}