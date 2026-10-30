#ifndef __MODEL_FV_001_RECORDS_H__
#define __MODEL_FV_001_RECORDS_H__

#include "fv_001_record.h"
#include <vector>

namespace model
{
    struct FV001Records
    {
        std::vector<FV001Record> records;

        template <class Reflector>
        void dg_reflect(const Reflector& reflector) const
        {
            reflector(records);
        }

        template <class Reflector>
        void dg_reflect(const Reflector& reflector)
        {
            reflector(records);
        }
    };
}

#endif