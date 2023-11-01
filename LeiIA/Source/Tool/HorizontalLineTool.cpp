// © 2023 Lei Cheng

#include "HorizontalLineTool.h"
#include "Layout.h"
#include "ToolType.h"

namespace lei
{
    HorizontalLineTool::HorizontalLineTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray) :
        component_(target_component),
        GetKArray_(GetKArray)
    {
        component_->setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }

    HorizontalLineTool::~HorizontalLineTool()
    {
    }

    void HorizontalLineTool::ToolBegin(const juce::Point<int>& position,
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

    void HorizontalLineTool::ToolProcess(const juce::Point<int>& position)
    {
        tool_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        component_->repaint();
    }

    void HorizontalLineTool::ToolEnd(const juce::Point<int>& position)
    {
        tool_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        confirmed_ = true;
        component_->repaint();
    }

    void HorizontalLineTool::ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                                         const std::pair<double, double>& min_max_label,
                                         const juce::Range<int>& scroll_bar_current_range,
                                         int bar_width)
    {
        chart_bounds_ = chart_bounds;
        min_max_label_ = min_max_label;
        scroll_bar_current_range_ = scroll_bar_current_range;
        bar_width_ = bar_width;
    }

    int HorizontalLineTool::GetChartIndex() const
    {
        return chart_index_;
    }

    juce::Uuid HorizontalLineTool::GetKey() const
    {
        return key_;
    }

    bool HorizontalLineTool::IsToolFinished() const
    {
        return confirmed_;
    }

    ToolType HorizontalLineTool::GetToolType() const
    {
        return ToolType::kHorizontalLine;
    }

    juce::Rectangle<int> HorizontalLineTool::GetEraseButtonBounds() const
    {
        const auto y = ToPositionY(tool_position_.second, chart_bounds_, min_max_label_);
        juce::Rectangle<int> erase_button(chart_bounds_.getX() - kToolEraseButtonWidthHeight / 2,
                                          y - kToolEraseButtonWidthHeight / 2,
                                          kToolEraseButtonWidthHeight,
                                          kToolEraseButtonWidthHeight);

        if (chart_bounds_.contains(erase_button.getCentre()))
        {
            return erase_button;
        }

        return {};
    }

    std::shared_ptr<juce::Button> HorizontalLineTool::GetEraseButton() const
    {
        if (!erase_button_)
        {
            erase_button_ = CreateEraseButton(key_.toString());
        }

        return erase_button_;
    }

    void HorizontalLineTool::Paint(juce::Graphics& g)
    {
        juce::Graphics::ScopedSaveState raii(g);

        g.reduceClipRegion(chart_bounds_);

        if (confirmed_)
        {
            g.setColour(juce::Colours::red);
        }
        else
        {
            g.setColour(juce::Colours::white);
        }

        g.drawHorizontalLine(ToPositionY(tool_position_.second, chart_bounds_, min_max_label_), chart_bounds_.getX(), chart_bounds_.getRight());
    }
}
