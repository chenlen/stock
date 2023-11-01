// © 2023 Lei Cheng

#include "DataCenter.h"
#include "rapidcsv.h"
#include <filesystem>

namespace lei
{
    juce::Time ConvertFromDateStringAndTimeString(const std::string& date_str, const std::string& time_str)
    {
        int year = 0;
        const auto pos_end_year = date_str.find_first_of('/');
        if (pos_end_year != std::string::npos)
        {
            year = std::stoi(date_str.substr(0, pos_end_year));
        }

        int month = 0;
        int day = 0;
        const auto pos_end_month = date_str.find_last_of('/');
        if (pos_end_month != std::string::npos)
        {
            month = std::stoi(date_str.substr(pos_end_year + 1, pos_end_month - pos_end_year - 1)) - 1;
            day = std::stoi(date_str.substr(pos_end_month + 1));
        }

        int hours = 0;
        int minutes = 0;
        int seconds = 0;
        if (!time_str.empty())
        {
            const auto pos_end_hours = time_str.find_first_of(':');
            if (pos_end_hours != std::string::npos)
            {
                hours = std::stoi(time_str.substr(0, pos_end_hours));
            }

            const auto pos_end_minutes = time_str.find_last_of(':');
            if (pos_end_minutes != std::string::npos)
            {
                minutes = std::stoi(time_str.substr(pos_end_hours + 1, pos_end_minutes - pos_end_hours - 1));
                seconds = std::stoi(time_str.substr(pos_end_minutes + 1));
            }
        }

        return juce::Time(year, month, day, hours, minutes, seconds);
    }

    const KArray& KDataCenter::GetKData(const std::string& stock_id, DataFrequency frequency) const
    {
        const auto pos = cache_.find(std::make_pair(stock_id, frequency));
        if (pos != cache_.end())
        {
            return pos->second;
        }

        switch (frequency)
        {
        case lei::DataFrequency::k1Min:
            return cache_.emplace(std::make_pair(stock_id, frequency), GetMinDatas(stock_id)).first->second;
        case lei::DataFrequency::kDay:
            return cache_.emplace(std::make_pair(stock_id, frequency), GetDayDatas(stock_id)).first->second;
        default:
            break;
        }

        return {};
    }

    KArray KDataCenter::GetDayDatas(const std::string& stock_id) const
    {
        const auto pos = stock_id.find_last_of('.');
        if (pos == std::string::npos)
        {
            return {};
        }

        const std::filesystem::path path(stock_id.substr(pos + 1) + "/day_k/" + stock_id.substr(0, pos) + ".csv");
        if (!std::filesystem::exists(path))
        {
            return {};
        }

        rapidcsv::Document doc(path.string());
        return std::make_tuple(doc.GetColumn<juce::Time>("Date", [](const std::string& date_str, juce::Time& t)
                                                         {
                                                             t = ConvertFromDateStringAndTimeString(date_str, "");
                                                         }),
                               doc.GetColumn<double>("Open"),
                               doc.GetColumn<double>("High"),
                               doc.GetColumn<double>("Low"),
                               doc.GetColumn<double>("Close"),
                               doc.GetColumn<unsigned long long>("Volume"));
    }

    KArray KDataCenter::GetMinDatas(const std::string& stock_id) const
    {
        const auto pos = stock_id.find_last_of('.');
        if (pos == std::string::npos)
        {
            return {};
        }

        const std::filesystem::path path(stock_id.substr(pos + 1) + "/min_k/" + stock_id.substr(0, pos) + ".csv");
        if (!std::filesystem::exists(path))
        {
            return {};
        }

        rapidcsv::Document doc(path.string());

        const auto count = doc.GetRowCount();
        DateTimeArray date_time_array;
        date_time_array.reserve(count);
        for (size_t i = 0; i < count; ++i)
        {
            date_time_array.emplace_back(ConvertFromDateStringAndTimeString(doc.GetCell<std::string>("Date", i),
                                                                            doc.GetCell<std::string>("Time", i)));
        }

        return std::make_tuple(date_time_array,
                               doc.GetColumn<double>("Open"),
                               doc.GetColumn<double>("High"),
                               doc.GetColumn<double>("Low"),
                               doc.GetColumn<double>("Close"),
                               doc.GetColumn<unsigned long long>("Volume"));
    }

    const KDataCenter& GetKDataCenter()
    {
        static KDataCenter instance;
        return instance;
    }
}
