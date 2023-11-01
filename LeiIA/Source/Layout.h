// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>
#include "Data/DataCenter.h"

namespace lei
{
    enum
    {
        kToolbarHeight = 32,
        kHeaderHeight = 40,
        kTimeLabelHeight = 40,
        kLeftLabelWidth = 10,
        kRightLabelWidth = 55,
        kSubsidiaryChartHeightPercentage = 20,
        kChartGap = 2,
        kWidgetsHeight = 16,
        kZoomButtonWidth = 16,
        kToolGap = 4,
        kStockSearchBarWidth = 70,
        kStockSearchBarHeight = 28,
        kDataFrequencyButtonWidth = 70,
        kDataFrequencyButtonHeight = 28,
        kToolButtonWidth = 70,
        kToolButtonHeight = 28,
        kChartBorderThickness = 1,
        kDefaultBarWidth = 11,
        kBarGap = 2,
        kToolEraseButtonWidthHeight = 16
    };

    juce::Time ToKTime(const juce::Point<int>& pt,
                       const std::function<const KArray& ()>& GetKArray,
                       const juce::Range<int>& scroll_bar_current_range,
                       const juce::Rectangle<int>& chart_bounds,
                       int bar_width);

    double ToPrice(const juce::Point<int>& pt, const juce::Rectangle<int>& chart_bounds, const std::pair<double, double>& min_max_label);

    int ToPositionX(const juce::Time& k_time,
                    const std::function<const KArray& ()>& GetKArray,
                    const juce::Range<int>& scroll_bar_current_range,
                    const juce::Rectangle<int>& chart_bounds,
                    int bar_width);

    int ToPositionX(const juce::Time& k_time,
                    int offsets,
                    const std::function<const KArray& ()>& GetKArray,
                    const juce::Range<int>& scroll_bar_current_range,
                    const juce::Rectangle<int>& chart_bounds,
                    int bar_width);

    int ToPositionY(double price, const juce::Rectangle<int>& chart_bounds, const std::pair<double, double>& min_max_label);
    int KTimeToKIndex(const juce::Time& k_time, const std::function<const KArray& ()>& GetKArray);
    juce::Point<int> GetBoundsPoint(const juce::Point<int>& pt, const juce::Rectangle<int>& bounds);
}
