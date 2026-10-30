#ifndef __MODEL_CSV_WORK_ORDER_H__
#define __MODEL_CSV_WORK_ORDER_H__

#include <stdint.h>
#include <stdlib.h>
#include <string>

namespace model
{
    struct CSVWorkOrder
    {
        std::string file_path;
        uint64_t read_off;
        uint64_t read_sz;
        bool skip_header;
        char delimiter;

        template <class Reflector>
        void dg_reflect(const Reflector& reflector) const
        {
            reflector(file_path,
                      read_off,
                      read_sz,
                      skip_header,
                      delimiter);
        }

        template <class Reflector>
        void dg_reflect(const Reflector& reflector)
        {
            reflector(file_path,
                      read_off,
                      read_sz,
                      skip_header,
                      delimiter);
        }
    };
}

#endif