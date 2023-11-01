// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>
#include "DataFrequency.h"
#include "Data/DataCenter.h"

namespace lei
{
    class WatchTool final
    {
    public:
        WatchTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray);
        ~WatchTool();

    public:
        void MouseEvent(const juce::Point<int>& position,
                        int k_index,
                        const juce::Rectangle<int>& all_chart_bounds,
                        const juce::Rectangle<int>& chart_bounds,
                        const juce::Rectangle<int>& price_label_bounds,
                        const juce::Rectangle<int>& time_label_bounds,
                        const std::pair<double, double>& min_max_label,
                        lei::DataFrequency data_frequency,
                        int k_centre_x);

        void Paint(juce::Graphics& g);
        void Clear();

    private:
        juce::Component* component_;
        std::function<const KArray& ()> GetKArray_;
        juce::Rectangle<int> all_chart_bounds_;
        juce::Rectangle<int> chart_bounds_;
        juce::Rectangle<int> price_label_bounds_;
        juce::Rectangle<int> time_label_bounds_;
        std::pair<double, double> min_max_label_;
        juce::Point<int> mouse_position_;
        int k_index_ = 0;
        int k_centre_x_ = 0;
        lei::DataFrequency data_frequency_;
    };
}
