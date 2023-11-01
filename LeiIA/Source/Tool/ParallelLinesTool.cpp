// © 2023 Lei Cheng

#include "ParallelLinesTool.h"
#include "Layout.h"
#include "ToolType.h"

namespace lei
{
    ParallelLinesTool::ParallelLinesTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray) :
        component_(target_component),
        GetKArray_(GetKArray)
    {
        component_->setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }

    ParallelLinesTool::~ParallelLinesTool()
    {
    }

    void ParallelLinesTool::ToolBegin(const juce::Point<int>& position,
                                      const juce::Rectangle<int>& chart_bounds,
                                      const std::pair<double, double>& min_max_label,
                                      const juce::Range<int>& scroll_bar_current_range,
                                      int bar_width,
                                      int chart_index)
    {
        if (second_position_confirmed_)
        {
            ToolBeginPhase2(position);
        }
        else
        {
            ToolBeginPhase1(position, chart_bounds, min_max_label, scroll_bar_current_range, bar_width, chart_index);
        }
    }

    void ParallelLinesTool::ToolProcess(const juce::Point<int>& position)
    {
        if (second_position_confirmed_)
        {
            ToolProcessPhase2(position);
        }
        else
        {
            ToolProcessPhase1(position);
        }
    }

    void ParallelLinesTool::ToolEnd(const juce::Point<int>& position)
    {
        if (second_position_confirmed_)
        {
            ToolEndPhase2(position);
        }
        else
        {
            ToolEndPhase1(position);
        }
    }

    void ParallelLinesTool::ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                                        const std::pair<double, double>& min_max_label,
                                        const juce::Range<int>& scroll_bar_current_range,
                                        int bar_width)
    {
        chart_bounds_ = chart_bounds;
        min_max_label_ = min_max_label;
        scroll_bar_current_range_ = scroll_bar_current_range;
        bar_width_ = bar_width;
    }

    int ParallelLinesTool::GetChartIndex() const
    {
        return chart_index_;
    }

    juce::Uuid ParallelLinesTool::GetKey() const
    {
        return key_;
    }

    bool ParallelLinesTool::IsToolFinished() const
    {
        return third_position_confirmed_;
    }

    ToolType ParallelLinesTool::GetToolType() const
    {
        return ToolType::kParallelLines;
    }

    juce::Rectangle<int> ParallelLinesTool::GetEraseButtonBounds() const
    {
        const auto x1 = ToPositionX(first_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        const auto y1 = ToPositionY(first_position_.second, chart_bounds_, min_max_label_);
        const auto x2 = ToPositionX(second_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        const auto y2 = ToPositionY(second_position_.second, chart_bounds_, min_max_label_);
        const auto line = GetExtendLine(chart_bounds_, { x1, y1 }, { x2, y2 }).toFloat();

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
                return { juce::roundToInt(intersection_point.getX() - kToolEraseButtonWidthHeight / 2),
                         juce::roundToInt(intersection_point.getY() - kToolEraseButtonWidthHeight / 2),
                         kToolEraseButtonWidthHeight,
                         kToolEraseButtonWidthHeight };
            }
        }

        const auto x3 = ToPositionX(third_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        const auto y3 = ToPositionY(third_position_.second, chart_bounds_, min_max_label_);
        for (const auto& line : GetParallelLines({ x1, y1, x2, y2 }, { x3, y3 }))
        {
            for (const auto& border_line : border_lines)
            {
                if (line.toFloat().intersects(border_line, intersection_point))
                {
                    return { juce::roundToInt(intersection_point.getX() - kToolEraseButtonWidthHeight / 2),
                             juce::roundToInt(intersection_point.getY() - kToolEraseButtonWidthHeight / 2),
                             kToolEraseButtonWidthHeight,
                             kToolEraseButtonWidthHeight };
                }
            }
        }

        return {};
    }

    std::shared_ptr<juce::Button> ParallelLinesTool::GetEraseButton() const
    {
        if (!erase_button_)
        {
            erase_button_ = CreateEraseButton(key_.toString());
        }

        return erase_button_;
    }

    void ParallelLinesTool::Paint(juce::Graphics& g)
    {
        juce::Graphics::ScopedSaveState raii(g);

        g.reduceClipRegion(chart_bounds_);

        if (second_position_confirmed_)
        {
            g.setColour(juce::Colours::magenta);
        }
        else
        {
            g.setColour(juce::Colours::white);
        }

        const juce::Point<int> pt1(ToPositionX(first_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_),
                                   ToPositionY(first_position_.second, chart_bounds_, min_max_label_));

        const juce::Point<int> pt2(ToPositionX(second_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_),
                                   ToPositionY(second_position_.second, chart_bounds_, min_max_label_));

        g.drawLine(GetExtendLine(chart_bounds_, pt1, pt2).toFloat());

        if (phase2_start_)
        {
            if (third_position_confirmed_)
            {
                g.setColour(juce::Colours::magenta);
            }
            else
            {
                g.setColour(juce::Colours::white);
            }

            const juce::Point<int> pt3(ToPositionX(third_position_.first, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_),
                                       ToPositionY(third_position_.second, chart_bounds_, min_max_label_));

            for (const auto& line : GetParallelLines({ pt1, pt2 }, pt3))
            {
                g.drawLine(line.toFloat());
            }
        }
    }

    void ParallelLinesTool::ToolBeginPhase1(const juce::Point<int>& position,
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

    void ParallelLinesTool::ToolProcessPhase1(const juce::Point<int>& position)
    {
        second_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        second_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        component_->repaint();
    }

    void ParallelLinesTool::ToolEndPhase1(const juce::Point<int>& position)
    {
        second_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        second_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        second_position_confirmed_ = true;
        component_->repaint();
    }

    void ParallelLinesTool::ToolBeginPhase2(const juce::Point<int>& position)
    {
        phase2_start_ = true;
        third_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        third_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        component_->repaint();
    }

    void ParallelLinesTool::ToolProcessPhase2(const juce::Point<int>& position)
    {
        third_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        third_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        component_->repaint();
    }

    void ParallelLinesTool::ToolEndPhase2(const juce::Point<int>& position)
    {
        third_position_.first = ToKTime(GetBoundsPoint(position, chart_bounds_), GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        third_position_.second = ToPrice(GetBoundsPoint(position, chart_bounds_), chart_bounds_, min_max_label_);
        third_position_confirmed_ = true;
        component_->repaint();
    }

    std::vector<juce::Line<int>> ParallelLinesTool::GetParallelLines(const juce::Line<int>& line, const juce::Point<int>& pt) const
    {
        std::vector<juce::Line<int>> lines;
        if (line.getStartX() == line.getEndX())
        {
            const auto distance = std::abs(pt.getX() - line.getStartX());

            juce::Point<int> p1(line.getStartX() + distance, chart_bounds_.getY());
            juce::Point<int> p2(line.getEndX() + distance, chart_bounds_.getBottom());
            while (chart_bounds_.intersects(juce::Line<int>(p1, p2)))
            {
                lines.emplace_back(p1, p2);
                p1.x += distance;
                p2.x += distance;
            }

            p1.x = line.getStartX() - distance;
            p2.x = line.getEndX() - distance;
            while (chart_bounds_.intersects(juce::Line<int>(p1, p2)))
            {
                lines.emplace_back(p1, p2);
                p1.x -= distance;
                p2.x -= distance;
            }
        }
        else if (line.getStartY() == line.getEndY())
        {
            const auto distance = std::abs(pt.getY() - line.getStartY());

            juce::Point<int> p1(chart_bounds_.getX(), line.getStartY() + distance);
            juce::Point<int> p2(chart_bounds_.getRight(), line.getEndY() + distance);
            while (chart_bounds_.intersects(juce::Line<int>(p1, p2)))
            {
                lines.emplace_back(p1, p2);
                p1.y += distance;
                p2.y += distance;
            }

            p1.y = line.getStartY() - distance;
            p2.y = line.getEndY() - distance;
            while (chart_bounds_.intersects(juce::Line<int>(p1, p2)))
            {
                lines.emplace_back(p1, p2);
                p1.y -= distance;
                p2.y -= distance;
            }
        }
        else
        {
            // y = mx + b
            const auto m = (line.getEndY() - line.getStartY()) / static_cast<double>(line.getEndX() - line.getStartX());
            const auto b = line.getStartY() - m * line.getStartX();

            // y = mx + b'
            const auto b_prime = -m * pt.getX() + pt.getY();
            const auto distance = std::abs(b_prime - b);

            juce::Point<double> p1(chart_bounds_.getX(), m * chart_bounds_.getX() + b + distance);
            juce::Point<double> p2(chart_bounds_.getRight(), m * chart_bounds_.getRight() + b + distance);
            while (chart_bounds_.intersects(juce::Line<int>(p1.roundToInt(), p2.roundToInt())))
            {
                lines.emplace_back(p1.roundToInt(), p2.roundToInt());
                p1.y += distance;
                p2.y += distance;
            }

            p1.y = m * chart_bounds_.getX() + b - distance;
            p2.y = m * chart_bounds_.getRight() + b - distance;
            while (chart_bounds_.intersects(juce::Line<int>(p1.roundToInt(), p2.roundToInt())))
            {
                lines.emplace_back(p1.roundToInt(), p2.roundToInt());
                p1.y -= distance;
                p2.y -= distance;
            }
        }

        return lines;
    }
}
