// © 2023 Lei Cheng

#include "TimeFibonacciSequenceTool.h"
#include "Layout.h"
#include "ToolType.h"

namespace lei
{
    TimeFibonacciSequenceTool::TimeFibonacciSequenceTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray) :
        component_(target_component),
        GetKArray_(GetKArray)
    {
        component_->setMouseCursor(juce::MouseCursor::CrosshairCursor);
    }

    TimeFibonacciSequenceTool::~TimeFibonacciSequenceTool()
    {
    }

    void TimeFibonacciSequenceTool::ToolBegin(const juce::Point<int>& position,
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

    void TimeFibonacciSequenceTool::ToolProcess(const juce::Point<int>& position)
    {
        tool_position_.first = ToKTime(position, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        component_->repaint();
    }

    void TimeFibonacciSequenceTool::ToolEnd(const juce::Point<int>& position)
    {
        tool_position_.first = ToKTime(position, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
        confirmed_ = true;
        component_->repaint();
    }

    void TimeFibonacciSequenceTool::ZoomChanged(const juce::Rectangle<int>& chart_bounds,
                                                const std::pair<double, double>& min_max_label,
                                                const juce::Range<int>& scroll_bar_current_range,
                                                int bar_width)
    {
        chart_bounds_ = chart_bounds;
        min_max_label_ = min_max_label;
        scroll_bar_current_range_ = scroll_bar_current_range;
        bar_width_ = bar_width;
    }

    int TimeFibonacciSequenceTool::GetChartIndex() const
    {
        return chart_index_;
    }

    juce::Uuid TimeFibonacciSequenceTool::GetKey() const
    {
        return key_;
    }

    bool TimeFibonacciSequenceTool::IsToolFinished() const
    {
        return confirmed_;
    }

    ToolType TimeFibonacciSequenceTool::GetToolType() const
    {
        return ToolType::kTimeFibonacciSequence;
    }

    juce::Rectangle<int> TimeFibonacciSequenceTool::GetEraseButtonBounds() const
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

    std::shared_ptr<juce::Button> TimeFibonacciSequenceTool::GetEraseButton() const
    {
        if (!erase_button_)
        {
            erase_button_ = CreateEraseButton(key_.toString());
        }

        return erase_button_;
    }

    void TimeFibonacciSequenceTool::Paint(juce::Graphics& g)
    {
        juce::Graphics::ScopedSaveState raii(g);

        const auto font = g.getCurrentFont().withStyle(juce::Font::bold);
        g.reduceClipRegion(chart_bounds_.withHeight(chart_bounds_.getHeight() + lei::kChartBorderThickness + font.getHeight()));
        g.setFont(font);

        if (confirmed_)
        {
            g.setColour(juce::Colours::chocolate);
        }
        else
        {
            g.setColour(juce::Colours::white);
        }

        const auto k_index = KTimeToKIndex(tool_position_.first, GetKArray_);
        for (const auto& fibonacci_number : FibonacciSequence(scroll_bar_current_range_.getEnd() - k_index - 1))
        {
            const auto x = ToPositionX(tool_position_.first, fibonacci_number, GetKArray_, scroll_bar_current_range_, chart_bounds_, bar_width_);
            g.drawVerticalLine(x, chart_bounds_.getY(), chart_bounds_.getBottom());

            const juce::String label_string(fibonacci_number);
            const auto width = font.getStringWidthFloat(label_string);
            g.drawText(label_string,
                       juce::Rectangle<float>(x - width / 2, chart_bounds_.getBottom() + lei::kChartBorderThickness, width, font.getHeight()),
                       juce::Justification::centred,
                       false);
        }
    }

    std::vector<int> TimeFibonacciSequenceTool::FibonacciSequence(int find_fibonacci_number)
    {
        std::vector<int> fibonacci_sequence;

        // Rule: Fib(n) = Fib(n-1) + Fib(n-2), Fib(0) = 0, Fib(1) = 1
        int Fib_n = 0; // Fib(n)
        int Fib_n1 = 1; // Fib(n-1)
        int Fib_n2 = 0; // Fib(n-2)

        if (Fib_n2 <= find_fibonacci_number)
        {
            fibonacci_sequence.push_back(Fib_n2);
        }

        if (Fib_n1 <= find_fibonacci_number)
        {
            fibonacci_sequence.push_back(Fib_n1);
        }

        Fib_n = Fib_n1 + Fib_n2;
        while (Fib_n <= find_fibonacci_number)
        {
            fibonacci_sequence.push_back(Fib_n);

            Fib_n2 = Fib_n1;
            Fib_n1 = Fib_n;
            Fib_n = Fib_n1 + Fib_n2;
        }

        return fibonacci_sequence;
    }
}
