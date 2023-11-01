// © 2023 Lei Cheng

#include "DrawUtility.h"
#include "Indicator/IndicatorType.h"
#include "K.h"
#include "Layout.h"

namespace lei
{
    K::K(const std::function<const KArray& ()>& GetKArray) :
        GetKArray_(GetKArray)
    {
    }

    K::~K()
    {
    }

    lei::IndicatorType K::GetIndicatorType() const
    {
        return IndicatorType::kK;
    }

    void K::StockChanged()
    {
        min_max_label_ = {};
    }

    void K::Calculate(const juce::Range<int>& scroll_bar_current_range)
    {
        const auto& low_array = std::get<3>(GetKArray_());
        const auto pos_low = std::min_element(low_array.begin() + scroll_bar_current_range.getStart(),
                                              low_array.begin() + scroll_bar_current_range.getEnd());

        if (pos_low != low_array.end())
        {
            min_max_label_.first = *pos_low;
        }

        const auto& high_array = std::get<2>(GetKArray_());
        const auto pos_high = std::max_element(high_array.begin() + scroll_bar_current_range.getStart(),
                                               high_array.begin() + scroll_bar_current_range.getEnd());

        if (pos_high != high_array.end())
        {
            min_max_label_.second = *pos_high;
        }
    }

    std::pair<double, double> K::GetMinMaxLabelValue() const
    {
        return min_max_label_;
    }

    void K::Draw(juce::Graphics& g,
                 juce::Rectangle<int> chart_bounds,
                 juce::Rectangle<int> label_bounds,
                 int bar_width,
                 const juce::Range<int>& scroll_bar_current_range,
                 const std::pair<double, double>& min_max_label)
    {
        DrawXGridAndLabel(g, chart_bounds, label_bounds, min_max_label);
        DrawKBar(g,
                 chart_bounds,
                 std::get<1>(GetKArray_()),
                 std::get<2>(GetKArray_()),
                 std::get<3>(GetKArray_()),
                 std::get<4>(GetKArray_()),
                 scroll_bar_current_range,
                 min_max_label,
                 bar_width);
    }

    void K::DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index)
    {
        if (k_index == -1)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);

        juce::String open_str("--");
        juce::String high_str("--");
        juce::String low_str("--");
        juce::String close_str("--");
        juce::String up_down_str("--");
        juce::String up_down_percentage_str("--%");

        const auto& close_array = std::get<4>(GetKArray_());
        if (!close_array.empty() && k_index < close_array.size())
        {
            open_str = juce::String(std::get<1>(GetKArray_())[k_index]);
            high_str = juce::String(std::get<2>(GetKArray_())[k_index]);
            low_str = juce::String(std::get<3>(GetKArray_())[k_index]);

            const auto close = close_array[k_index];
            close_str = juce::String(close);

            if (k_index > 0)
            {
                const auto pre_close = close_array[k_index - 1];
                const auto up_down = close - pre_close;
                up_down_str = juce::String(up_down);
                up_down_percentage_str = juce::String::formatted("%+.2f%%", up_down / pre_close * 100);
            }
        }

        const auto message = juce::translate("open") + " " + open_str + " " +
            juce::translate("high") + " " + high_str + " " +
            juce::translate("low") + " " + low_str + " " +
            juce::translate("close") + " " + close_str + " " +
            juce::translate("up down") + " " + up_down_str + " " +
            juce::translate("up down percentage") + " " + up_down_percentage_str;

        g.setColour(juce::Colours::white);
        g.setFont(GetWatchToolMessageFont());
        chart_bounds.removeFromLeft(1);
        g.drawText(message, chart_bounds, juce::Justification::topLeft, false);
    }

    void K::DrawXGridAndLabel(juce::Graphics& g,
                              juce::Rectangle<int> chart_bounds,
                              juce::Rectangle<int> label_bounds,
                              const std::pair<double, double>& min_max_label)
    {
        juce::Graphics::ScopedSaveState raii(g);

        const auto font = GetValueLabelFont();
        const auto font_height = font.getHeight();
        const auto chart_height = chart_bounds.getHeight();
        const int kDefGridSize = 9;
        const int max_grid_size = (chart_height - font_height) / font_height;
        const int grid_size = std::min(kDefGridSize, max_grid_size);
        const auto price_per_grid = (min_max_label.second - min_max_label.first) / (grid_size + 1);
        const auto height_per_grid = static_cast<float>(chart_height) / (grid_size + 1);

        g.setColour(juce::Colours::grey);
        for (int i = 1; i <= grid_size; ++i)
        {
            g.drawHorizontalLine(juce::roundToInt(chart_bounds.getY() + height_per_grid * i),
                                 chart_bounds.getX(),
                                 chart_bounds.getRight());
        }

        g.setFont(font);
        g.setColour(juce::Colours::white);
        for (int i = 1; i <= grid_size; ++i)
        {
            g.drawText(juce::String(min_max_label.second - price_per_grid * i),
                       juce::Rectangle<float>(label_bounds.getX(),
                                              label_bounds.getY() + height_per_grid * i - font_height / 2,
                                              label_bounds.getWidth(),
                                              font_height),
                       juce::Justification::centredLeft,
                       false);
        }
    }

    void K::DrawKBar(juce::Graphics& g,
                     juce::Rectangle<int> chart_bounds,
                     const OpenArray& open_array,
                     const HighArray& high_array,
                     const LowArray& low_array,
                     const CloseArray& close_array,
                     const juce::Range<int>& scroll_bar_current_range,
                     const std::pair<double, double>& min_max_label,
                     int bar_width)
    {
        if (min_max_label.first == min_max_label.second)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);

        const auto ratio = chart_bounds.getHeight() / (min_max_label.second - min_max_label.first);
        const auto begin = scroll_bar_current_range.getStart();
        const auto end = scroll_bar_current_range.getEnd();
        for (int i = begin; i < end; ++i)
        {
            const auto open = open_array[i];
            const auto close = close_array[i];
            g.setColour(close > open ? juce::Colours::red : (close < open ? juce::Colours::green : juce::Colours::white));

            chart_bounds.removeFromLeft(kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);
            g.fillRect(bar_bounds.getX(),
                       juce::roundToInt(std::min(bar_bounds.getY() + (min_max_label.second - std::max(open, close)) * ratio, bar_bounds.getBottom() - 1.0)),
                       bar_bounds.getWidth(),
                       juce::roundToInt(std::max(std::abs(close - open) * ratio, 1.0)));

            g.drawVerticalLine(bar_bounds.getCentreX(),
                               bar_bounds.getY() + (min_max_label.second - high_array[i]) * ratio,
                               bar_bounds.getY() + (min_max_label.second - low_array[i]) * ratio);
        }
    }
}
