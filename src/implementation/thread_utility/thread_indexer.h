#ifndef __THREAD_UTILITY_THREAD_INDEXER_H__
#define __THREAD_UTILITY_THREAD_INDEXER_H__

#include <stdint.h>
#include <stdlib.h>
#include <thread>
#include <atomic>
#include <optional>

namespace thread_utility
{
    class ThreadIndexer
    {
        private:

            static inline std::atomic<size_t> incrementor               = 0;
            static inline thread_local std::optional<size_t> thr_idx    = std::nullopt;

        public:

            static inline auto get_index() -> size_t
            {
                if (!thr_idx.has_value())
                {
                    thr_idx = incrementor.fetch_add(1u, std::memory_order_relaxed);
                }

                return thr_idx.value();
            }
    };
}

#endif