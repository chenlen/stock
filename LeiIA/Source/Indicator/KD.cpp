// © 2023 Lei Cheng

#include "DrawUtility.h"
#include "Indicator/IndicatorType.h"
#include "KD.h"
#include "Layout.h"

namespace lei
{
    KD::KD(const std::function<const KArray& ()>& GetKArray, int period, int rsv_weight, int k_weight) :
        GetKArray_(GetKArray),
        period_(period),
        rsv_weight_(rsv_weight),
        k_weight_(k_weight)
    {
        jassert(GetKArray_);
        jassert(period_ >= 2);
        jassert(rsv_weight_ >= 2);
        jassert(k_weight_ >= 2);
    }

    KD::~KD()
    {
    }

    lei::IndicatorType KD::GetIndicatorType() const
    {
        return IndicatorType::kKD;
    }

    void KD::StockChanged()
    {
        rsv_array_.clear();
        k_array_.clear();
        d_array_.clear();
        recalculate_ = true;
    }

    void KD::Calculate(const juce::Range<int>& scroll_bar_current_range)
    {
        if (!recalculate_)
        {
            return;
        }

        if (period_ < 2 || rsv_weight_ < 2 || k_weight_ < 2)
        {
            return;
        }

        rsv_array_.clear();
        k_array_.clear();
        d_array_.clear();

        const auto& high_array = std::get<2>(GetKArray_());
        const auto& low_array = std::get<3>(GetKArray_());
        const auto& close_array = std::get<4>(GetKArray_());

        const auto size = close_array.size();
        rsv_array_.reserve(size);
        k_array_.reserve(size);
        d_array_.reserve(size);

        for (int i = 0; i < period_ - 1; ++i)
        {
            rsv_array_.push_back(50);
            k_array_.push_back(50);
            d_array_.push_back(50);
        }

        for (int i = period_ - 1; i < size; ++i)
        {
            const auto min = *std::min_element(low_array.begin() + i + 1 - period_, low_array.begin() + i + 1);
            const auto max = *std::max_element(high_array.begin() + i + 1 - period_, high_array.begin() + i + 1);
            rsv_array_.push_back(min != max ? (close_array[i] - min) / (max - min) * 100 : 50);
            k_array_.push_back(k_array_[i - 1] * (rsv_weight_ - 1) / rsv_weight_ + rsv_array_[i] * 1 / rsv_weight_);
            d_array_.push_back(d_array_[i - 1] * (k_weight_ - 1) / k_weight_ + k_array_[i] * 1 / k_weight_);
        }

        recalculate_ = false;
    }

    std::pair<double, double> KD::GetMinMaxLabelValue() const
    {
        return std::make_pair(0, 100);
    }

    void KD::Draw(juce::Graphics& g,
                  juce::Rectangle<int> chart_bounds,
                  juce::Rectangle<int> label_bounds,
                  int bar_width,
                  const juce::Range<int>& scroll_bar_current_range,
                  const std::pair<double, double>& min_max_label)
    {
        DrawXGridAndLabel(g, chart_bounds, label_bounds);
        DrawLine(g, chart_bounds, bar_width, scroll_bar_current_range, min_max_label, juce::Colours::yellow, k_array_, period_);
        DrawLine(g, chart_bounds, bar_width, scroll_bar_current_range, min_max_label, juce::Colours::orange, d_array_, period_);
    }

    void KD::DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index)
    {
        if (k_index == -1)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);
        const auto font = GetWatchToolMessageFont();
        g.setFont(font);

        juce::String k_message("K(" + juce::String(period_) + ", " + juce::String(rsv_weight_) + ") ");
        juce::String d_message("D(" + juce::String(period_) + ", " + juce::String(k_weight_) + ") ");
        if (k_index >= period_ - 2)
        {
            k_message += juce::String::formatted("%.2f%%", k_array_[k_index]);
            d_message += juce::String::formatted("%.2f%%", d_array_[k_index]);
        }
        else
        {
            k_message += "--%";
            d_message += "--%";
        }

        g.setColour(juce::Colours::yellow);
        chart_bounds.removeFromLeft(1);
        g.drawText(k_message, chart_bounds.removeFromLeft(font.getStringWidth(k_message)), juce::Justification::topLeft, false);

        g.setColour(juce::Colours::orange);
        chart_bounds.removeFromLeft(10);
        g.drawText(d_message, chart_bounds.removeFromLeft(font.getStringWidth(d_message)), juce::Justification::topLeft, false);
    }

    void KD::DrawLine(juce::Graphics& g,
                      juce::Rectangle<int> chart_bounds,
                      int bar_width,
                      const juce::Range<int>& scroll_bar_current_range,
                      const std::pair<double, double>& min_max_label,
                      const juce::Colour& line_color,
                      const std::vector<double>& data_array,
                      int period)
    {
        if (data_array.size() < scroll_bar_current_range.getEnd() || min_max_label.first == min_max_label.second)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(line_color);

        const auto ratio = chart_bounds.getHeight() / (min_max_label.second - min_max_label.first);
        const auto begin = data_array.cbegin() + scroll_bar_current_range.getStart();
        const auto end = data_array.cbegin() + scroll_bar_current_range.getEnd();

        bool first_point = true;
        juce::Path path;
        for (auto it = begin; it != end; ++it)
        {
            chart_bounds.removeFromLeft(kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);

            if (std::distance(data_array.cbegin(), it) < period - 2)
            {
                continue;
            }

            if (first_point)
            {
                path.startNewSubPath(bar_bounds.getCentreX(), bar_bounds.getY() + (min_max_label.second - *it) * ratio);
                first_point = false;
            }
            else
            {
                path.lineTo(bar_bounds.getCentreX(), bar_bounds.getY() + (min_max_label.second - *it) * ratio);
            }
        }

        g.strokePath(path, juce::PathStrokeType(1));
    }

    void KD::DrawXGridAndLabel(juce::Graphics& g, juce::Rectangle<int> chart_bounds, juce::Rectangle<int> label_bounds)
    {
        juce::Graphics::ScopedSaveState raii(g);

        const std::array<int, 3> label_values = { 20, 50, 80 };

        g.setColour(juce::Colours::grey);
        for (const auto& label_value : label_values)
        {
            g.drawHorizontalLine(juce::roundToInt(chart_bounds.getBottom() - chart_bounds.getHeight() * label_value / 100.0),
                                 chart_bounds.getX(),
                                 chart_bounds.getRight());
        }

        const auto font = GetValueLabelFont();
        const auto font_height = font.getHeight();
        g.setFont(font);
        g.setColour(juce::Colours::white);
        for (const auto& label_value : label_values)
        {
            g.drawText(juce::String(label_value),
                       juce::Rectangle<float>(label_bounds.getX(),
                                              label_bounds.getBottom() - label_bounds.getHeight() * label_value / 100.0 - font_height / 2,
                                              label_bounds.getWidth(),
                                              font_height),
                       juce::Justification::centredLeft,
                       false);
        }
    }
}
