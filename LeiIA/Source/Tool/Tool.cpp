// © 2023 Lei Cheng

#include "Tool.h"
#include "ToolType.h"

namespace lei
{
    juce::Line<int> GetExtendLine(const juce::Rectangle<int>& chart_bounds, const juce::Point<int>& pt1, const juce::Point<int>& pt2)
    {
        if (pt1.getX() == pt2.getX())
        {
            return { pt1.getX(), chart_bounds.getY(), pt2.getX(), chart_bounds.getBottom() };
        }
        else if (pt1.getY() == pt2.getY())
        {
            return { chart_bounds.getX(), pt1.getY(), chart_bounds.getRight(), pt2.getY() };
        }
        else
        {
            // y = mx + b
            const auto m = (pt2.getY() - pt1.getY()) / static_cast<double>(pt2.getX() - pt1.getX());
            const auto b = pt1.getY() - m * pt1.getX();
            return { chart_bounds.getX(),
                     juce::roundToInt(m * chart_bounds.getX() + b),
                     chart_bounds.getRight(),
                     juce::roundToInt(m * chart_bounds.getRight() + b) };
        }
    }

    std::shared_ptr<juce::Button> CreateEraseButton(const juce::String& tool_key)
    {
        juce::Path shape;
        shape.addLineSegment(juce::Line<float>(0.0f, 0.0f, 1.0f, 1.0f), 0.35f);
        shape.addLineSegment(juce::Line<float>(1.0f, 0.0f, 0.0f, 1.0f), 0.35f);

        auto button = std::make_shared<juce::ShapeButton>("erase tool", juce::Colour(0xffff0000), juce::Colour(0xd7ff0000), juce::Colour(0xf7ff0000));
        button->setShape(shape, true, true, false);
        button->setButtonText(tool_key);
        return button;
    }

    bool IsAcrossCharts(ToolType tool_type)
    {
        switch (tool_type)
        {
        case ToolType::kVerticalLine:
            return true;
        case ToolType::kTimeFibonacciSequence:
            return true;
        default:
            break;
        }

        return false;
    }
}
