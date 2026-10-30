#ifndef __REDUCE_DATASTRUCUTRE_REDUCE_MEMORY_H__
#define __REDUCE_DATASTRUCTURE_REDUCE_MEMORY_H__

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <type_traits>
#include <cstring>
#include <global_config/reduce_memory.h>
#include <implementation/memory_allocator/mmap_memory_allocator.h>
#include <implementation/memory_allocator/std_memory_allocator.h>

namespace reduce_datastructure
{
    struct Signature{}; 

    using MemoryAllocatorSingleton  = stdx::singleton<Signature,
                                                      std::shared_ptr<memory_allocator::MemoryAllocatorInterface>>;

    void init()
    {

    }

    void deinit() noexcept
    {

    }

    template <class T>
    class StlMemoryAllocator
    {
        private:

            static consteval auto is_no_align_malloc() -> bool
            {
                return alignof(T) <= 1;
            }

        public:

            using value_type                                = T;
            using pointer                                   = T*;
            using const_pointer                             = const T*;
            using reference                                 = T&;
            using const_reference                           = const T&;
            using size_type                                 = std::size_t;
            using difference_type                           = std::ptrdiff_t;
            using propagate_on_container_move_assignment    = std::true_type;
            using is_always_equal                           = std::true_type; // Stateless

            template <class U>
            struct rebind
            {
                using other = StlMemoryAllocator<U>; 
            };

            constexpr StlMemoryAllocator() noexcept = default;
            constexpr StlMemoryAllocator(const StlMemoryAllocator&) noexcept = default;
            
            template <class U>
            constexpr StlMemoryAllocator(const StlMemoryAllocator<U>&) noexcept {}
            
            ~StlMemoryAllocator() = default;

            auto allocate(std::size_t n) -> pointer
            {
                if constexpr(is_no_align_malloc())
                {
                    return static_cast<pointer>(global_allocator::dg_malloc(sizeof(T) * n));
                }
                else
                {
                    return static_cast<pointer>(global_allocator::dg_aligned_alloc(alignof(T), sizeof(T) * n));
                }
            }

            void deallocate(T* p, std::size_t n) noexcept
            {
                if constexpr(is_no_align_malloc())
                {
                    [[clang::noinline]] global_allocator::dg_free(p);
                }
                else
                {
                    [[clang::noinline]] global_allocator::dg_aligned_free(p);
                }
            }
    };

    template <class T, class T1>
    constexpr auto operator==(const StlMemoryAllocator<T>&, const StlMemoryAllocator<T1>&) noexcept -> bool
    {
        return true;
    }

    template<class T, class T1>
    constexpr auto operator!=(const StlMemoryAllocator<T>&, const StlMemoryAllocator<T1>&) noexcept -> bool
    {
        return false;
    }
}

#endif