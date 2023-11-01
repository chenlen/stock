// © 2023 Lei Cheng

#pragma once

#include "Tool/Tool.h"

namespace lei
{
    class EraseTool : public Tool
    {
    public:
        EraseTool(juce::Component* target_component,
                  const std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>>& tools,
                  const std::function<void(const std::shared_ptr<juce::Button>&)>& RegisterEraseButton,
                  const std::function<void(const std::shared_ptr<juce::Button>&)>& UnregisterEraseButton);

        ~EraseTool() override;

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
        std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>> tools_;
        std::function<void(const std::shared_ptr<juce::Button>&)> RegisterEraseButton_;
        std::function<void(const std::shared_ptr<juce::Button>&)> UnregisterEraseButton_;
    };
}
