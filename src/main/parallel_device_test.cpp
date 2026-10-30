#define STRONG_MEMORY_ORDERING_FLAG true

#include <implementation/parallel_device/finite_parallel_task_launcher_factory.h> 
#include <implementation/parallel_device/lambda_as_task.h>
#include <random>
#include <utility>
#include <functional>
#include <chrono>
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

void run_one_test()
{
    const size_t CONCURRENT_WORKER_SZ_RANGE     = size_t{1} << 4;
    const size_t JOB_QUEUE_CAP_RANGE            = size_t{1} << 4;
    const size_t INCREMENT_TASK_SZ_RANGE        = size_t{1} << 4; 

    const std::chrono::milliseconds YIELD_TIME  = std::chrono::milliseconds(1);

    size_t concurrent_worker_sz                 = randomize_int(0u, CONCURRENT_WORKER_SZ_RANGE) + 1u;
    size_t job_queue_cap                        = randomize_int(0u, JOB_QUEUE_CAP_RANGE) + 1u;
    size_t increment_task_sz                    = randomize_int(0u, INCREMENT_TASK_SZ_RANGE);

    auto factory    = parallel_device::FiniteParallelTaskLauncherFactory();

    factory.set_concurrent_worker_size(concurrent_worker_sz);
    factory.set_job_queue_capacity(job_queue_cap);

    std::unique_ptr<parallel_device::ParallelTaskLauncherInterface> task_launcher   = factory.get();

    std::atomic<size_t> incrementor         = 0u;

    for (size_t i = 0u; i < increment_task_sz; ++i)
    {
        auto increment_task = [&]() noexcept
        {
            incrementor.fetch_add(1u, std::memory_order_relaxed);
        };

        task_launcher->launch
        (
            parallel_device::lambda_as_task(increment_task)
        );
    }

    while (incrementor.load(std::memory_order_relaxed) != increment_task_sz)
    {
        std::this_thread::sleep_for(YIELD_TIME);
    } 
}

void run_test()
{
    const size_t TEST_SZ    = size_t{1} << 14;
    const size_t REPORT_SZ  = size_t{1} << 4;

    std::cout << "__START_PARALLEL_DEVICE_TEST__\n";

    for (size_t i = 0u; i < TEST_SZ; ++i)
    {
        run_one_test();

        if (i % REPORT_SZ == 0u)
        {
            std::cout << i << "/" << TEST_SZ << "\n";
        }
    }

    std::cout << "__END_PARALLEL_DEVICE_TEST__\n";
}

int main()
{
    run_test();
}