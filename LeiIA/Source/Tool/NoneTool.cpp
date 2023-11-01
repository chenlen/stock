// © 2023 Lei Cheng

#include "NoneTool.h"
#include "ToolType.h"

namespace lei
{
    NoneTool::NoneTool(juce::Component* target_component)
    {
        target_component->setMouseCursor(juce::MouseCursor::NormalCursor);
    }

    NoneTool::~NoneTool()
    {
    }

    void NoneTool::ToolBegin(const juce::Point<int>& position,
                             const juce::Rectangle<int>& chart_bounds,
                             const std::pair<double, double>& min_max_label,
                             const juce::Range<int>& scroll_bar_current_range,
                             int bar_width,
                             int chart_index)
    {
    }

    void NoneTool::ToolProcess(const juce::Point<int>& position)
    {
    }

    void NoneTool::ToolEnd(const juce::Point<int>& position)
    {
    }

    void NoneTool::ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                               const std::pair<double, double>& min_max_label,
                               const juce::Range<int>& scroll_bar_current_range,
                               int bar_width)
    {
    }

    int NoneTool::GetChartIndex() const
    {
        return -1;
    }

    juce::Uuid NoneTool::GetKey() const
    {
        return {};
    }

    bool NoneTool::IsToolFinished() const
    {
        return false;
    }

    ToolType NoneTool::GetToolType() const
    {
        return ToolType::kNone;
    }

    juce::Rectangle<int> NoneTool::GetEraseButtonBounds() const
    {
        return {};
    }

    std::shared_ptr<juce::Button> NoneTool::GetEraseButton() const
    {
        return nullptr;
    }

    void NoneTool::Paint(juce::Graphics& g)
    {
    }
}
