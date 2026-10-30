#ifndef __RECORD_RECORD_ITERATOR_INTERFACE_H__
#define __RECORD_RECORD_ITERATOR_INTERFACE_H__

#include <stdint.h>
#include <stdlib.h>
#include <optional>

namespace iterator
{
    template <class T>
    class IteratorInterface
    {
        public:

            virtual ~IteratorInterface() noexcept = default;

            virtual auto next() -> std::optional<T> = 0;
    };
}

#endif