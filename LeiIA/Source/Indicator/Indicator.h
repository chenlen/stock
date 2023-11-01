// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>

namespace lei
{
    enum class IndicatorType;

    class Indicator
    {
    public:
        virtual ~Indicator() = 0
        {}

    public:
        virtual IndicatorType GetIndicatorType() const = 0;

        virtual void StockChanged() = 0;

        virtual void Calculate(const juce::Range<int>& scroll_bar_current_range) = 0;

        virtual std::pair<double, double> GetMinMaxLabelValue() const = 0;

        virtual void Draw(juce::Graphics& g,
                          juce::Rectangle<int> chart_bounds,
                          juce::Rectangle<int> label_bounds,
                          int bar_width,
                          const juce::Range<int>& scroll_bar_current_range,
                          const std::pair<double, double>& min_max_label) = 0;

        virtual void DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index) = 0;
    };
}
