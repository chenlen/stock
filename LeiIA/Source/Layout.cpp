// © 2023 Lei Cheng

#include "Layout.h"

namespace lei
{
    juce::Time ToKTime(const juce::Point<int>& pt,
                       const std::function<const KArray& ()>& GetKArray,
                       const juce::Range<int>& scroll_bar_current_range,
                       const juce::Rectangle<int>& chart_bounds,
                       int bar_width)
    {
        if (scroll_bar_current_range.getLength() == 0)
        {
            return {};
        }

        int index = scroll_bar_current_range.getStart() + (pt.getX() - chart_bounds.getX() - lei::kBarGap / 2.0) / (bar_width + lei::kBarGap);
        index = std::max(index, scroll_bar_current_range.getStart());
        index = std::min(index, scroll_bar_current_range.getEnd() - 1);
        return std::get<0>(GetKArray())[index];
    }

    double ToPrice(const juce::Point<int>& pt, const juce::Rectangle<int>& chart_bounds, const std::pair<double, double>& min_max_label)
    {
        const auto price_per_height = (min_max_label.second - min_max_label.first) / chart_bounds.getHeight();
        const auto price = min_max_label.second - price_per_height * (pt.getY() - chart_bounds.getY());
        return price;
    }

    int ToPositionX(const juce::Time& k_time,
                    const std::function<const KArray& ()>& GetKArray,
                    const juce::Range<int>& scroll_bar_current_range,
                    const juce::Rectangle<int>& chart_bounds,
                    int bar_width)
    {
        const auto& date_time_array = std::get<0>(GetKArray());
        const auto index = std::distance(date_time_array.begin(), std::lower_bound(date_time_array.begin(), date_time_array.end(), k_time));
        const auto distance = index - scroll_bar_current_range.getStart();
        return chart_bounds.getX() + lei::kBarGap + distance * (bar_width + lei::kBarGap) + bar_width / 2.0; // don't use round to int, because bar_bounds.getCentreX in K::DrawKBar not use.
    }

    int ToPositionX(const juce::Time& k_time,
                    int offsets,
                    const std::function<const KArray& ()>& GetKArray,
                    const juce::Range<int>& scroll_bar_current_range,
                    const juce::Rectangle<int>& chart_bounds,
                    int bar_width)
    {
        const auto& date_time_array = std::get<0>(GetKArray());
        const auto index = std::distance(date_time_array.begin(), std::lower_bound(date_time_array.begin(), date_time_array.end(), k_time));
        const auto distance = index + offsets - scroll_bar_current_range.getStart();
        return chart_bounds.getX() + lei::kBarGap + distance * (bar_width + lei::kBarGap) + bar_width / 2.0; // don't use round to int, because bar_bounds.getCentreX in K::DrawKBar not use.
    }

    int ToPositionY(double price, const juce::Rectangle<int>& chart_bounds, const std::pair<double, double>& min_max_label)
    {
        const auto height_per_price = chart_bounds.getHeight() / (min_max_label.second - min_max_label.first);
        const auto y = chart_bounds.getY() + height_per_price * (min_max_label.second - price);
        return juce::roundToInt(y);
    }

    int KTimeToKIndex(const juce::Time& k_time, const std::function<const KArray& ()>& GetKArray)
    {
        const auto& date_time_array = std::get<0>(GetKArray());
        return std::distance(date_time_array.begin(), std::lower_bound(date_time_array.begin(), date_time_array.end(), k_time));
    }

    juce::Point<int> GetBoundsPoint(const juce::Point<int>& pt, const juce::Rectangle<int>& bounds)
    {
        return { std::min(std::max(pt.getX(), bounds.getX()), bounds.getRight() - 1),
                 std::min(std::max(pt.getY(), bounds.getY()), bounds.getBottom() - 1) };
    }
}
