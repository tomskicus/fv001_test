#ifndef __FV_001_ACCUMULATABLE_VALUE_H__
#define __FV_001_ACCUMULATABLE_VALUE_H__

#include <stdint.h>
#include <stdlib.h>

namespace model
{
    struct FV001AccumulatableValue
    {
        uint64_t impressions;
        uint64_t clicks;
        float spend;
        uint64_t conversions;

        template <class Reflector>
        void dg_reflect(const Reflector& reflector) const
        {
            reflector(impressions, clicks, spend, conversions);
        }

        template <class Reflector>
        void dg_reflect(const Reflector& reflector)
        {
            reflector(impressions, clicks, spend, conversions);
        }
    };
}

#endif