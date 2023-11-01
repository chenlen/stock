// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>

namespace lei
{
    enum class ToolType;

    juce::Line<int> GetExtendLine(const juce::Rectangle<int>& chart_bounds, const juce::Point<int>& pt1, const juce::Point<int>& pt2);
    std::shared_ptr<juce::Button> CreateEraseButton(const juce::String& tool_key);
    bool IsAcrossCharts(ToolType tool_type);

    using ToolPosition = std::pair<juce::Time, double>;

    class Tool
    {
    public:
        virtual ~Tool() = 0
        {}

    public:
        virtual void ToolBegin(const juce::Point<int>& position,
                               const juce::Rectangle<int>& chart_bounds,
                               const std::pair<double, double>& min_max_label,
                               const juce::Range<int>& scroll_bar_current_range,
                               int bar_width,
                               int chart_index) = 0;

        virtual void ToolProcess(const juce::Point<int>& position) = 0;
        virtual void ToolEnd(const juce::Point<int>& position) = 0;

        virtual void ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                                 const std::pair<double, double>& min_max_label,
                                 const juce::Range<int>& scroll_bar_current_range,
                                 int bar_width) = 0;

        virtual int GetChartIndex() const = 0;
        virtual juce::Uuid GetKey() const = 0;
        virtual bool IsToolFinished() const = 0;
        virtual ToolType GetToolType() const = 0;
        virtual juce::Rectangle<int> GetEraseButtonBounds() const = 0;
        virtual std::shared_ptr<juce::Button> GetEraseButton() const = 0;
        virtual void Paint(juce::Graphics& g) = 0;
    };
}
