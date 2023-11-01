// © 2023 Lei Cheng

#include "KChart/KChart.h"
#include "DrawUtility.h"
#include "Layout.h"

namespace lei
{
    KChart::KChart()
    {
    }

    KChart::~KChart()
    {
    }

    void KChart::DrawHeader(juce::Graphics& g,
                            const juce::Rectangle<int>& header_bounds,
                            const std::string& stock_id,
                            const lei::DateTimeArray& date_time_array,
                            const lei::OpenArray& open_array,
                            const lei::HighArray& high_array,
                            const lei::LowArray& low_array,
                            const lei::CloseArray& close_array,
                            const lei::VolumeArray& volume_array,
                            const juce::Font& font,
                            DataFrequency frequency) const
    {
        if (date_time_array.empty())
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);

        const auto close = *close_array.rbegin();
        juce::String up_down_str("--");
        juce::String up_down_percentage_str("--%");
        if (close_array.size() >= 2)
        {
            const auto pre_close = *(++close_array.rbegin());
            const auto up_down = close - pre_close;
            up_down_str = juce::String(up_down);
            up_down_percentage_str = juce::String::formatted("%+.2f%%", up_down / pre_close * 100);
        }

        const juce::String header_string = juce::translate(stock_id) + "(" + stock_id + ") " +
            date_time_array.rbegin()->formatted(GetTimeFormat(frequency)) + " " +
            juce::translate("open") + " " + juce::String(*open_array.rbegin()) + " " +
            juce::translate("high") + " " + juce::String(*high_array.rbegin()) + " " +
            juce::translate("low") + " " + juce::String(*low_array.rbegin()) + " " +
            juce::translate("close") + " " + juce::String(close) + " " +
            juce::translate("volume") + " " + juce::String(*volume_array.rbegin()) + " " +
            juce::translate("up down") + " " + up_down_str + " " +
            juce::translate("up down percentage") + " " + up_down_percentage_str;

        g.setColour(juce::Colours::white);
        g.setFont(font);

        const int kXGap = 1;
        g.drawText(header_string, header_bounds.reduced(kXGap, 0), juce::Justification::centredLeft, false);
    }

    void KChart::DrawBounds(juce::Graphics& g, juce::Rectangle<int> chart_bounds) const
    {
        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(juce::Colours::white);
        g.drawRect(chart_bounds, kChartBorderThickness);
    }

    void KChart::DrawTimeGrid(juce::Graphics& g,
                              juce::Rectangle<int> chart_bounds,
                              const DateTimeArray& date_time_array,
                              const juce::Range<int>& scroll_bar_current_range,
                              int bar_width,
                              DataFrequency frequency) const
    {
        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(juce::Colours::grey);

        const auto begin = scroll_bar_current_range.getStart();
        const auto end = scroll_bar_current_range.getEnd();
        for (int i = begin; i < end; ++i)
        {
            chart_bounds.removeFromLeft(kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);
            if (frequency == lei::DataFrequency::kDay)
            {
                if (i == begin || date_time_array[i].getMonth() == date_time_array[i - 1].getMonth())
                {
                    continue;
                }
            }
            else if (frequency == lei::DataFrequency::k1Min)
            {
                if (i == begin || date_time_array[i].getHours() == date_time_array[i - 1].getHours())
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            g.drawVerticalLine(bar_bounds.getCentreX(), bar_bounds.getY(), bar_bounds.getBottom());
        }
    }

    void KChart::DrawTimeLabel(juce::Graphics& g,
                               juce::Rectangle<int> chart_bounds,
                               const juce::Rectangle<int>& time_label_bounds,
                               const lei::DateTimeArray& date_time_array,
                               const juce::Range<int>& scroll_bar_current_range,
                               int bar_width,
                               DataFrequency frequency) const
    {
        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(juce::Colours::white);

        const auto font = lei::GetTimeLabelFont();
        g.setFont(font);

        const auto begin = scroll_bar_current_range.getStart();
        const auto end = scroll_bar_current_range.getEnd();
        for (int i = begin; i < end; ++i)
        {
            chart_bounds.removeFromLeft(lei::kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);
            if (frequency == lei::DataFrequency::kDay)
            {
                if (i == begin || date_time_array[i].getMonth() == date_time_array[i - 1].getMonth())
                {
                    continue;
                }
            }
            else if (frequency == lei::DataFrequency::k1Min)
            {
                if (i == begin || date_time_array[i].getHours() == date_time_array[i - 1].getHours())
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            const auto label_string = date_time_array[i].formatted(GetTimeFormat(frequency));
            const auto width = font.getStringWidthFloat(label_string);
            g.drawText(label_string,
                       juce::Rectangle<float>(bar_bounds.getCentreX() - width / 2,
                                              time_label_bounds.getY(),
                                              width,
                                              time_label_bounds.getHeight()),
                       juce::Justification::centred,
                       false);
        }
    }
}
