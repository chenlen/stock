// © 2023 Lei Cheng

#include "VerticalLineTool.h"
#include "Layout.h"
#include "ToolType.h"

namespace lei
{
    VerticalLineTool::VerticalLineTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray) :
        component_(target_component),
        GetKArray_(GetKArray)
    {
        component_->setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }

    VerticalLineTool::~VerticalLineTool()
    {
    }

    void VerticalLineTool::ToolBegin(const juce::Point<int>& position,
                                     const juce::Rectangle<int>& chart_bounds,
                                     const std::pair<double, double>& min_max_label,
                                     const juce::Range<int>& scroll_bar_current_range,
                                     int bar_width,
                                     int chart_index)
    {
        if (!chart_bounds.contains(position))
        {
            return;
        }

        chart_bounds_ = chart_bounds;
        min_max_label_ = min_max_label;
        scroll_bar_current_range_ = scroll_bar_current_range;
        bar_width_ = bar_width;
        chart_index_ = chart_index;
        tool_position_.first = ToKTime(position, GetKArray_, scroll_bar_current_range, chart_bounds, bar_width);
        tool_position_.second = ToPrice(position, chart_bounds, min_max_label);
        component_->repaint();
    }

    void VerticalLineTool::ToolProcess(const juce::Point<int>& position)
    {
        tool_position_.first = ToKTime(position, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        component_->repaint();
    }

    void VerticalLineTool::ToolEnd(const juce::Point<int>& position)
    {
        tool_position_.first = ToKTime(position, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        confirmed_ = true;
        component_->repaint();
    }

    void VerticalLineTool::ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                                       const std::pair<double, double>& min_max_label,
                                       const juce::Range<int>& scroll_bar_current_range,
                                       int bar_width)
    {
        chart_bounds_ = chart_bounds;
        min_max_label_ = min_max_label;
        scroll_bar_current_range_ = scroll_bar_current_range;
        bar_width_ = bar_width;
    }

    int VerticalLineTool::GetChartIndex() const
    {
        return chart_index_;
    }

    juce::Uuid VerticalLineTool::GetKey() const
    {
        return key_;
    }

    bool VerticalLineTool::IsToolFinished() const
    {
        return confirmed_;
    }

    ToolType VerticalLineTool::GetToolType() const
    {
        return ToolType::kVerticalLine;
    }

    juce::Rectangle<int> VerticalLineTool::GetEraseButtonBounds() const
    {
        const auto x = ToPositionX(tool_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        juce::Rectangle<int> erase_button(x - kToolEraseButtonWidthHeight / 2,
                                          chart_bounds_.getY() - kToolEraseButtonWidthHeight / 2,
                                          kToolEraseButtonWidthHeight,
                                          kToolEraseButtonWidthHeight);

        if (chart_bounds_.contains(erase_button.getCentre()))
        {
            return erase_button;
        }

        return {};
    }

    std::shared_ptr<juce::Button> VerticalLineTool::GetEraseButton() const
    {
        if (!erase_button_)
        {
            erase_button_ = CreateEraseButton(key_.toString());
        }

        return erase_button_;
    }

    void VerticalLineTool::Paint(juce::Graphics& g)
    {
        juce::Graphics::ScopedSaveState raii(g);

        g.reduceClipRegion(chart_bounds_);

        if (confirmed_)
        {
            g.setColour(juce::Colours::orange);
        }
        else
        {
            g.setColour(juce::Colours::white);
        }

        g.drawVerticalLine(ToPositionX(tool_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_),
                           chart_bounds_.getY(),
                           chart_bounds_.getBottom());
    }
}
