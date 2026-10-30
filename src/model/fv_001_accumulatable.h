#ifndef __MODEL_FV_001_ACCUMUTABLE_H__
#define __MODEL_FV_001_ACCUMUTABLE_H__

#include <stdint.h>
#include <stdlib.h>
#include <lib/stl_extension/datastructure.h>
#include <lib/stl_extension/hasher.h>
#include <type_traits>
#include <numeric>
#include "fv_001_accumulatable_value.h"

namespace model
{
    using fv_001_unordered_map  = datastructure::unordered_map_variants::unordered_node_map
    <
        std::string,
        FV001AccumulatableValue,
        size_t,
        std::integral_constant<bool, true>
    >;

    struct FV001Accumulatable
    {
        fv_001_unordered_map accumulatable_map;

        template <class Reflector>
        void dg_reflect(const Reflector& reflector) const
        {
            reflector(accumulatable_map);
        }

        template <class Reflector>
        void dg_reflect(const Reflector& reflector)
        {
            reflector(accumulatable_map);
        }
    };
}

#endif