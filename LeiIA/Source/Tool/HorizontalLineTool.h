// © 2023 Lei Cheng

#pragma once

#include "Tool/Tool.h"
#include "Data/DataCenter.h"

namespace lei
{
    class HorizontalLineTool : public Tool
    {
    public:
        HorizontalLineTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray);
        ~HorizontalLineTool() override;

    public:
        void ToolBegin(const juce::Point<int>& position,
                       const juce::Rectangle<int>& chart_bounds,
                       const std::pair<double, double>& min_max_label,
                       const juce::Range<int>& scroll_bar_current_range,
                       int bar_width,
                       int chart_index) override;

        void ToolProcess(const juce::Point<int>& position) override;
        void ToolEnd(const juce::Point<int>& position) override;

        void ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                         const std::pair<double, double>& min_max_label,
                         const juce::Range<int>& scroll_bar_current_range,
                         int bar_width) override;

        int GetChartIndex() const override;
        juce::Uuid GetKey() const override;
        bool IsToolFinished() const override;
        ToolType GetToolType() const override;
        juce::Rectangle<int> GetEraseButtonBounds() const override;
        std::shared_ptr<juce::Button> GetEraseButton() const override;
        void Paint(juce::Graphics& g) override;

    private:
        juce::Component* component_;
        std::function<const KArray& ()> GetKArray_;
        ToolPosition tool_position_;
        juce::Rectangle<int> chart_bounds_;
        std::pair<double, double> min_max_label_;
        juce::Range<int> scroll_bar_current_range_;
        int bar_width_;
        juce::Uuid key_;
        int chart_index_ = 0;
        bool confirmed_ = false;
        mutable std::shared_ptr<juce::Button> erase_button_;
    };
}
