// © 2023 Lei Cheng

#include "DrawUtility.h"
#include "Indicator/IndicatorType.h"
#include "Layout.h"
#include "Volume.h"

namespace lei
{
    Volume::Volume(const std::function<const KArray& ()>& GetKArray) :
        GetKArray_(GetKArray),
        min_max_label_()
    {
    }

    Volume::~Volume()
    {
    }

    lei::IndicatorType Volume::GetIndicatorType() const
    {
        return IndicatorType::kVolume;
    }

    void Volume::StockChanged()
    {
        min_max_label_ = {};
    }

    void Volume::Calculate(const juce::Range<int>& scroll_bar_current_range)
    {
        const auto& volume_array = std::get<5>(GetKArray_());
        const auto [min, max] = std::minmax_element(volume_array.begin() + scroll_bar_current_range.getStart(),
                                                    volume_array.begin() + scroll_bar_current_range.getEnd());

        if (min != volume_array.end() && max != volume_array.end())
        {
            min_max_label_ = std::make_pair(*min, *max);
        }
    }

    std::pair<double, double> Volume::GetMinMaxLabelValue() const
    {
        return min_max_label_;
    }

    void Volume::Draw(juce::Graphics& g,
                      juce::Rectangle<int> chart_bounds,
                      juce::Rectangle<int> label_bounds,
                      int bar_width,
                      const juce::Range<int>& scroll_bar_current_range,
                      const std::pair<double, double>& min_max_label)
    {
        DrawXGridAndLabel(g, chart_bounds, label_bounds, min_max_label.second);
        DrawVolumeBar(g, chart_bounds, std::get<5>(GetKArray_()), std::get<4>(GetKArray_()), scroll_bar_current_range, min_max_label.second, bar_width);
    }

    void Volume::DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index)
    {
        if (k_index == -1)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(juce::Colours::white);
        g.setFont(GetWatchToolMessageFont());
        chart_bounds.removeFromLeft(1);
        g.drawText(juce::translate("volume chart") + " " + juce::String(std::get<5>(GetKArray_())[k_index]),
                   chart_bounds,
                   juce::Justification::topLeft,
                   false);
    }

    void Volume::DrawXGridAndLabel(juce::Graphics& g,
                                   juce::Rectangle<int> chart_bounds,
                                   juce::Rectangle<int> label_bounds,
                                   unsigned long long max_volume)
    {
        juce::Graphics::ScopedSaveState raii(g);

        auto font = GetValueLabelFont();
        const auto font_height = font.getHeight();
        const auto chart_height = chart_bounds.getHeight();
        const int kDefGridSize = 4;
        const int max_grid_size = (chart_height - font_height) / font_height;
        const int grid_size = std::min(kDefGridSize, max_grid_size);
        const auto price_per_grid = juce::roundToInt(max_volume / static_cast<double>(grid_size + 1));
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
            g.drawText(juce::String(max_volume - price_per_grid * i),
                       juce::Rectangle<float>(label_bounds.getX(),
                                              label_bounds.getY() + height_per_grid * i - font_height / 2,
                                              label_bounds.getWidth(),
                                              font_height),
                       juce::Justification::centredLeft,
                       false);
        }
    }

    void Volume::DrawVolumeBar(juce::Graphics& g,
                               juce::Rectangle<int> chart_bounds,
                               const VolumeArray& volume_array,
                               const CloseArray& close_array,
                               const juce::Range<int>& scroll_bar_current_range,
                               unsigned long long max_volume,
                               int bar_width)
    {
        if (max_volume == 0)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);

        const auto ratio = static_cast<double>(chart_bounds.getHeight()) / max_volume;
        const auto begin = scroll_bar_current_range.getStart();
        const auto end = scroll_bar_current_range.getEnd();
        for (int i = begin; i < end; ++i)
        {
            const auto close = close_array[i];
            const auto pre_close = (i == 0 ? close : close_array[i - 1]);
            g.setColour(close > pre_close ? juce::Colours::red : (close < pre_close ? juce::Colours::green : juce::Colours::white));

            chart_bounds.removeFromLeft(kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);
            const auto volume = volume_array[i];
            g.fillRect(bar_bounds.getX(),
                       juce::roundToInt(std::min(bar_bounds.getY() + (max_volume - volume) * ratio, bar_bounds.getBottom() - 1.0)),
                       bar_bounds.getWidth(),
                       juce::roundToInt(std::max(volume * ratio, 1.0)));
        }
    }
}
