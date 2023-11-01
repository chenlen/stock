// © 2023 Lei Cheng

#include "DrawUtility.h"
#include "Indicator/IndicatorType.h"
#include "Layout.h"
#include "MA.h"

namespace lei
{
    MA::MA(const std::function<const KArray& ()>& GetKArray, int period, const juce::Colour& color) :
        GetKArray_(GetKArray),
        period_(period),
        color_(color)
    {
        jassert(GetKArray_);
        jassert(period_ >= 1);
    }

    MA::~MA()
    {
    }

    lei::IndicatorType MA::GetIndicatorType() const
    {
        return IndicatorType::kMA;
    }

    void MA::StockChanged()
    {
        min_max_label_ = {};
        ma_array_.clear();
        recalculate_ = true;
    }

    void MA::Calculate(const juce::Range<int>& scroll_bar_current_range)
    {
        if (!recalculate_)
        {
            min_max_label_ = CalculateMinMaxLabel(scroll_bar_current_range);
            return;
        }

        const auto& close_array = std::get<4>(GetKArray_());
        const auto size = close_array.size();

        if (period_ < 1 || period_ > size)
        {
            return;
        }

        ma_array_.clear();
        ma_array_.reserve(size);

        for (int i = 0; i < period_ - 1; ++i)
        {
            ma_array_.push_back(0);
        }

        for (int i = period_ - 1; i < size; ++i)
        {
            ma_array_.push_back(std::accumulate(close_array.begin() + i + 1 - period_, close_array.begin() + i + 1, 0.0) / period_);
        }

        min_max_label_ = CalculateMinMaxLabel(scroll_bar_current_range);
        recalculate_ = false;
    }

    std::pair<double, double> MA::GetMinMaxLabelValue() const
    {
        return min_max_label_;
    }

    void MA::Draw(juce::Graphics& g,
                  juce::Rectangle<int> chart_bounds,
                  juce::Rectangle<int> label_bounds,
                  int bar_width,
                  const juce::Range<int>& scroll_bar_current_range,
                  const std::pair<double, double>& min_max_label)
    {
        if (min_max_label.first == min_max_label.second)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(color_);

        const auto ratio = chart_bounds.getHeight() / (min_max_label.second - min_max_label.first);
        const auto end = scroll_bar_current_range.getEnd();

        bool first_point = true;
        juce::Path path;
        for (int i = scroll_bar_current_range.getStart(); i < end; ++i)
        {
            chart_bounds.removeFromLeft(kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);

            if (i < period_ - 1)
            {
                continue;
            }

            if (first_point)
            {
                path.startNewSubPath(bar_bounds.getCentreX(), bar_bounds.getY() + (min_max_label.second - ma_array_[i]) * ratio);
                first_point = false;
            }
            else
            {
                path.lineTo(bar_bounds.getCentreX(), bar_bounds.getY() + (min_max_label.second - ma_array_[i]) * ratio);
            }

            if (i + period_ == end)
            {
                const auto triangle_height = sqrt(3) / 2 * bar_width;
                g.drawLine(bar_bounds.getX(), bar_bounds.getBottom(), bar_bounds.getCentreX(), bar_bounds.getBottom() - triangle_height);
                g.drawLine(bar_bounds.getRight(), bar_bounds.getBottom(), bar_bounds.getCentreX(), bar_bounds.getBottom() - triangle_height);
            }
        }

        g.strokePath(path, juce::PathStrokeType(1));
    }

    void MA::DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index)
    {
        if (k_index == -1)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);

        juce::String message("MA" + juce::String(period_) + " ");
        if (k_index >= period_ - 1)
        {
            message += juce::String(ma_array_[k_index]);
        }
        else
        {
            message += "--";
        }

        g.setColour(color_);
        g.setFont(GetWatchToolMessageFont());
        chart_bounds.removeFromLeft(1);
        g.drawText(message, chart_bounds, juce::Justification::topLeft, false);
    }

    std::pair<double, double> MA::CalculateMinMaxLabel(const juce::Range<int>& scroll_bar_current_range) const
    {
        const auto [min, max] = std::minmax_element(ma_array_.begin() + std::max(scroll_bar_current_range.getStart(), period_ - 1),
                                                    ma_array_.begin() + scroll_bar_current_range.getEnd());

        return std::make_pair(*min, *max);
    }
}
