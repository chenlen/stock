// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>
#include "Data/DataCenter.h"

namespace lei
{
    class KChart
    {
    public:
        KChart();
        ~KChart();

    public:
        void DrawHeader(juce::Graphics& g,
                        const juce::Rectangle<int>& header_bounds,
                        const std::string& stock_id,
                        const lei::DateTimeArray& date_time_array,
                        const lei::OpenArray& open_array,
                        const lei::HighArray& high_array,
                        const lei::LowArray& low_array,
                        const lei::CloseArray& close_array,
                        const lei::VolumeArray& volume_array,
                        const juce::Font& font,
                        DataFrequency frequency) const;

        void DrawBounds(juce::Graphics& g, juce::Rectangle<int> chart_bounds) const;

        void DrawTimeGrid(juce::Graphics& g,
                          juce::Rectangle<int> chart_bounds,
                          const DateTimeArray& date_time_array,
                          const juce::Range<int>& scroll_bar_current_range,
                          int bar_width,
                          DataFrequency frequency) const;

        void DrawTimeLabel(juce::Graphics& g,
                           juce::Rectangle<int> chart_bounds,
                           const juce::Rectangle<int>& time_label_bounds,
                           const lei::DateTimeArray& date_time_array,
                           const juce::Range<int>& scroll_bar_current_range,
                           int bar_width,
                           DataFrequency frequency) const;
    };
}
