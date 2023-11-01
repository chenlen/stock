// © 2023 Lei Cheng

#include "LineTool.h"
#include "Layout.h"
#include "ToolType.h"

namespace lei
{
    LineTool::LineTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray) :
        component_(target_component),
        GetKArray_(GetKArray)
    {
        component_->setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }

    LineTool::~LineTool()
    {
    }

    void LineTool::ToolBegin(const juce::Point<int>& position,
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
        second_position_.first = first_position_.first = ToKTime(position, GetKArray_, scroll_bar_current_range, chart_bounds, bar_width);
        second_position_.second = first_position_.second = ToPrice(position, chart_bounds, min_max_label);
        component_->repaint();
    }

    void LineTool::ToolProcess(const juce::Point<int>& position)
    {
        second_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        second_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        component_->repaint();
    }

    void LineTool::ToolEnd(const juce::Point<int>& position)
    {
        second_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        second_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        confirmed_ = true;
        component_->repaint();
    }

    void LineTool::ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                               const std::pair<double, double>& min_max_label,
                               const juce::Range<int>& scroll_bar_current_range,
                               int bar_width)
    {
        chart_bounds_ = chart_bounds;
        min_max_label_ = min_max_label;
        scroll_bar_current_range_ = scroll_bar_current_range;
        bar_width_ = bar_width;
    }

    int LineTool::GetChartIndex() const
    {
        return chart_index_;
    }

    juce::Uuid LineTool::GetKey() const
    {
        return key_;
    }

    bool LineTool::IsToolFinished() const
    {
        return confirmed_;
    }

    ToolType LineTool::GetToolType() const
    {
        return ToolType::kLine;
    }

    juce::Rectangle<int> LineTool::GetEraseButtonBounds() const
    {
        const auto x1 = ToPositionX(first_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        const auto y1 = ToPositionY(first_position_.second, chart_bounds_, min_max_label_);
        juce::Rectangle<int> erase_button(x1 - kToolEraseButtonWidthHeight / 2,
                                          y1 - kToolEraseButtonWidthHeight / 2,
                                          kToolEraseButtonWidthHeight,
                                          kToolEraseButtonWidthHeight);

        if (chart_bounds_.contains(erase_button.getCentre()))
        {
            return erase_button;
        }

        const auto x2 = ToPositionX(second_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        const auto y2 = ToPositionY(second_position_.second, chart_bounds_, min_max_label_);
        erase_button.setBounds(x2 - kToolEraseButtonWidthHeight / 2,
                               y2 - kToolEraseButtonWidthHeight / 2,
                               kToolEraseButtonWidthHeight,
                               kToolEraseButtonWidthHeight);

        if (chart_bounds_.contains(erase_button.getCentre()))
        {
            return erase_button;
        }

        const juce::Line<float> line(x1, y1, x2, y2);

        const std::array<juce::Line<float>, 4> border_lines = {
            juce::Line<float>(chart_bounds_.getTopLeft().toFloat(), chart_bounds_.getBottomLeft().toFloat()),
            juce::Line<float>(chart_bounds_.getTopLeft().toFloat(), chart_bounds_.getTopRight().toFloat()),
            juce::Line<float>(chart_bounds_.getTopRight().toFloat(), chart_bounds_.getBottomRight().toFloat()),
            juce::Line<float>(chart_bounds_.getBottomLeft().toFloat(), chart_bounds_.getBottomRight().toFloat()) };

        juce::Point<float> intersection_point;
        for (const auto& border_line : border_lines)
        {
            if (line.intersects(border_line, intersection_point))
            {
                erase_button.setBounds(intersection_point.getX() - kToolEraseButtonWidthHeight / 2,
                                       intersection_point.getY() - kToolEraseButtonWidthHeight / 2,
                                       kToolEraseButtonWidthHeight,
                                       kToolEraseButtonWidthHeight);

                return erase_button;
            }
        }

        return {};
    }

    std::shared_ptr<juce::Button> LineTool::GetEraseButton() const
    {
        if (!erase_button_)
        {
            erase_button_ = CreateEraseButton(key_.toString());
        }

        return erase_button_;
    }

    void LineTool::Paint(juce::Graphics& g)
    {
        juce::Graphics::ScopedSaveState raii(g);

        g.reduceClipRegion(chart_bounds_);

        if (confirmed_)
        {
            g.setColour(juce::Colours::bisque);
        }
        else
        {
            g.setColour(juce::Colours::white);
        }

        g.drawLine(ToPositionX(first_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_),
                   ToPositionY(first_position_.second, chart_bounds_, min_max_label_),
                   ToPositionX(second_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_),
                   ToPositionY(second_position_.second, chart_bounds_, min_max_label_));
    }
}
