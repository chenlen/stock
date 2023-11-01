// © 2023 Lei Cheng

#pragma once

#include "Tool/Tool.h"
#include "Data/DataCenter.h"

namespace lei
{
    class ParallelLinesTool : public Tool
    {
    public:
        ParallelLinesTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray);
        ~ParallelLinesTool() override;

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
        void ToolBeginPhase1(const juce::Point<int>& position,
                             const juce::Rectangle<int>& chart_bounds,
                             const std::pair<double, double>& min_max_label,
                             const juce::Range<int>& scroll_bar_current_range,
                             int bar_width,
                             int chart_index);

        void ToolProcessPhase1(const juce::Point<int>& position);
        void ToolEndPhase1(const juce::Point<int>& position);

        void ToolBeginPhase2(const juce::Point<int>& position);
        void ToolProcessPhase2(const juce::Point<int>& position);
        void ToolEndPhase2(const juce::Point<int>& position);

        std::vector<juce::Line<int>> GetParallelLines(const juce::Line<int>& line, const juce::Point<int>& pt) const;

    private:
        juce::Component* component_;
        std::function<const KArray& ()> GetKArray_;
        ToolPosition first_position_;
        ToolPosition second_position_;
        ToolPosition third_position_;
        juce::Rectangle<int> chart_bounds_;
        std::pair<double, double> min_max_label_;
        juce::Range<int> scroll_bar_current_range_;
        int bar_width_;
        juce::Uuid key_;
        int chart_index_ = 0;
        bool phase2_start_ = false;
        bool second_position_confirmed_ = false;
        bool third_position_confirmed_ = false;
        mutable std::shared_ptr<juce::Button> erase_button_;
    };
}
