#ifndef __MODEL_FV_001_RECORD_H__
#define __MODEL_FV_001_RECORD_H__

namespace model
{
    struct FV001Record
    {
        std::string campaign_id;
        std::string date;
        
        uint64_t impressions;
        uint64_t clicks;
        float spend;
        uint64_t conversions;

        template <class Reflector>
        void dg_reflect(const Reflector& reflector) const
        {
            reflector(campaign_id, date, impressions, clicks, spend, conversions);
        }

        template <class Reflector>
        void dg_reflect(const Reflector& reflector)
        {
            reflector(campaign_id, date, impressions, clicks, spend, conversions);
        }
    };
}

#endif