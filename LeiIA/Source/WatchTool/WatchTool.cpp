// © 2023 Lei Cheng

#include "WatchTool/WatchTool.h"
#include "DrawUtility.h"
#include "Layout.h"

namespace lei
{
    WatchTool::WatchTool(juce::Component* target_component, const std::function<const KArray& ()>& GetKArray) :
        component_(target_component),
        GetKArray_(GetKArray)
    {

    }

    WatchTool::~WatchTool()
    {

    }

    void WatchTool::MouseEvent(const juce::Point<int>& position,
                               int k_index,
                               const juce::Rectangle<int>& all_chart_bounds,
                               const juce::Rectangle<int>& chart_bounds,
                               const juce::Rectangle<int>& price_label_bounds,
                               const juce::Rectangle<int>& time_label_bounds,
                               const std::pair<double, double>& min_max_label,
                               lei::DataFrequency data_frequency,
                               int k_centre_x)
    {
        k_index_ = k_index;
        all_chart_bounds_ = all_chart_bounds;
        if (!chart_bounds.isEmpty())
        {
            chart_bounds_ = chart_bounds;
            price_label_bounds_ = price_label_bounds;
            min_max_label_ = min_max_label;
        }

        time_label_bounds_ = time_label_bounds;
        data_frequency_ = data_frequency;
        k_centre_x_ = k_centre_x;
        if (mouse_position_ != position)
        {
            mouse_position_ = position;
            component_->repaint();
        }
    }

    void WatchTool::Paint(juce::Graphics& g)
    {
        if (k_index_ == -1)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);

        const auto y = std::min(std::max(mouse_position_.getY(), chart_bounds_.getY()),
                                chart_bounds_.getBottom() - 1);

        juce::Point pt(k_centre_x_, y);

        g.setColour(juce::Colours::aqua);
        g.drawVerticalLine(k_centre_x_, all_chart_bounds_.getY(), all_chart_bounds_.getBottom());
        g.drawHorizontalLine(y, all_chart_bounds_.getX(), all_chart_bounds_.getRight());

        const auto font = lei::GetWatchToolMessageFont();
        g.setFont(font);

        const auto font_height = font.getHeight();
        const auto ratio = (min_max_label_.second - min_max_label_.first) / chart_bounds_.getHeight();
        const auto price = min_max_label_.second - ratio * (y - chart_bounds_.getY());
        g.drawText(juce::String(price),
                   juce::Rectangle<float>(price_label_bounds_.getX(),
                                          y - font_height / 2.0,
                                          price_label_bounds_.getWidth(),
                                          font_height),
                   juce::Justification::centredLeft,
                   false);

        const auto& date_time_array = std::get<0>(GetKArray_());
        if (!date_time_array.empty() && k_index_ < date_time_array.size())
        {
            const auto time_label_string = date_time_array[k_index_].formatted(GetTimeFormat(data_frequency_));
            const auto time_label_width = font.getStringWidthFloat(time_label_string);
            auto x = std::min(k_centre_x_ - time_label_width / 2, component_->getRight() - time_label_width);
            x = std::max<float>(x, component_->getX());
            g.drawText(time_label_string,
                       juce::Rectangle<float>(x, time_label_bounds_.getY(), time_label_width, time_label_bounds_.getHeight()),
                       juce::Justification::centredTop,
                       false);
        }
    }

    void WatchTool::Clear()
    {
        time_label_bounds_ = price_label_bounds_ = chart_bounds_ = all_chart_bounds_ = {};
        min_max_label_ = {};
        mouse_position_ = {};
        k_centre_x_ = k_index_ = 0;
        data_frequency_ = {};
    }
}
