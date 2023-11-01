// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>
#include "DataFrequency.h"
#include "Key.h"

namespace lei
{
    using DateTimeArray = std::vector<juce::Time>;
    using OpenArray = std::vector<double>;
    using HighArray = std::vector<double>;
    using LowArray = std::vector<double>;
    using CloseArray = std::vector<double>;
    using VolumeArray = std::vector<unsigned long long>;
    using KType = std::tuple<juce::Time, double, double, double, double, unsigned long long>;
    using KArray = std::tuple<DateTimeArray, OpenArray, HighArray, LowArray, CloseArray, VolumeArray>;

    class KDataCenter final
    {
    public:
        KDataCenter() = default;
        ~KDataCenter() = default;

    public:
        const KArray& GetKData(const std::string& stock_id, DataFrequency frequency) const;

    private:
        KArray GetDayDatas(const std::string& stock_id) const;
        KArray GetMinDatas(const std::string& stock_id) const;

    private:
        mutable std::unordered_map<std::pair<std::string, DataFrequency>, KArray> cache_;
    };

    const KDataCenter& GetKDataCenter();
}
