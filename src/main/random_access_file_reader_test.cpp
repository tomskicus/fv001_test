#define STRONG_MEMORY_ORDERING_FLAG true
#define DEBUG_MODE_FLAG true

#include <implementation/file_reader/linux_random_access_file_reader.h>
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

auto randomize_memory(size_t sz) -> std::string
{
    std::string rs{};

    for (size_t i = 0u; i < sz; ++i)
    {
        rs.push_back
        (
            randomize_az_AZ_09()
        );
    }

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

auto get_test_file(const std::string& content) -> std::shared_ptr<std::filesystem::path>
{
    std::filesystem::path tmp_file_path(get_random_filename());

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

void run_one_test()
{
    const size_t INSPECTION_SZ      = size_t{1} << 10;
    const size_t MEM_EXP_SZ_RANGE   = 16;
    size_t mem_sz                   = randomize_int(0u, size_t{1} << randomize_int(0u, MEM_EXP_SZ_RANGE));

    std::string test_mem                                                = randomize_memory(mem_sz);
    std::shared_ptr<std::filesystem::path> file                         = get_test_file(test_mem);
    std::shared_ptr<memory::ReadableMemorySpanInterface> memory_span    = file_reader::LinuxRandomAccessFileReader().to_memory_span(*file);
 
    for (size_t i = 0u; i < INSPECTION_SZ; ++i)
    {
        auto [offset, sz]   = get_test_interval(mem_sz);

        std::string lhs     = std::string
        (
            std::next(test_mem.begin(), offset),
            std::next(test_mem.begin(), offset + sz)
        );

        std::string rhs     = memory_span->read(offset, sz);

        if (lhs != rhs)
        {
            std::cout << "mayday, mismatched memory\n";
            std::abort();
        }
    }
}

void run_test()
{
    const size_t TEST_SZ    = size_t{1} << 16;
    const size_t COUT_SZ    = size_t{1} << 4;

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