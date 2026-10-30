#ifndef __IMPLEMENTATION_FV_001_MAP_REDUCE_FV_001_REDUCE_RESOLUTOR_H__
#define __IMPLEMENTATION_FV_001_MAP_REDUCE_FV_001_REDUCE_RESOLUTOR

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <string_view>
#include <interface/job_map_reduce/job_reduce_resolutor_interface.h>
#include <model/fv_001_accumulatable.h>
#include <memory>

namespace fv_001_map_reduce
{
    class FV001ReduceResolutor: public virtual job_map_reduce::JobReduceResolutorInterface
    {
        public:

            inline auto reduce(const std::shared_ptr<void>& inp1,
                               const std::shared_ptr<void>& inp2) -> std::shared_ptr<void>
            {
                std::shared_ptr<model::FV001Accumulatable> accumulatable1   = std::static_pointer_cast<model::FV001Accumulatable>(inp1);
                std::shared_ptr<model::FV001Accumulatable> accumulatable2   = std::static_pointer_cast<model::FV001Accumulatable>(inp2);

                for (const auto& bucket: accumulatable2->accumulatable_map)
                {
                    auto it = accumulatable1->accumulatable_map.find(bucket.first);

                    if (it == accumulatable1->accumulatable_map.end())
                    {
                        accumulatable1->accumulatable_map.insert({bucket.first, bucket.second});
                    }
                    else
                    {
                        it->second.impressions += bucket.second.impressions;
                        it->second.clicks      += bucket.second.clicks;
                        it->second.spend       += bucket.second.spend;
                        it->second.conversions += bucket.second.conversions;
                    }
                }

                return inp1;
            }
    };
}

#endif