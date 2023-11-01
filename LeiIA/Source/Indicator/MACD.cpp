// © 2023 Lei Cheng

#include "DrawUtility.h"
#include "Indicator/IndicatorType.h"
#include "Layout.h"
#include "MACD.h"

namespace lei
{
    MACD::MACD(const std::function<const KArray& ()>& GetKArray, int ema_short_period, int ema_long_period, int macd_period) :
        GetKArray_(GetKArray),
        ema_short_period_(ema_short_period),
        ema_long_period_(ema_long_period),
        macd_period_(macd_period)
    {
        jassert(GetKArray_);
        jassert(ema_short_period_ > 0);
        jassert(ema_long_period_ > 0);
        jassert(ema_short_period_ < ema_long_period_);
        jassert(macd_period_ > 0);
    }

    MACD::~MACD()
    {
    }

    lei::IndicatorType MACD::GetIndicatorType() const
    {
        return IndicatorType::kMACD;
    }

    void MACD::StockChanged()
    {
        min_max_label_ = {};
        dif_array_.clear();
        macd_array_.clear();
        osc_array_.clear();
        recalculate_ = true;
    }

    void MACD::Calculate(const juce::Range<int>& scroll_bar_current_range)
    {
        if (!recalculate_)
        {
            min_max_label_ = CalculateMinMaxLabel(scroll_bar_current_range);
            return;
        }

        const auto& close_array = std::get<4>(GetKArray_());
        dif_array_ = DIF(EMA(close_array, ema_short_period_), ema_short_period_, EMA(close_array, ema_long_period_), ema_long_period_);
        macd_array_ = EMA(dif_array_, macd_period_);
        osc_array_ = OSC(dif_array_, ema_long_period_, macd_array_, macd_period_);
        min_max_label_ = CalculateMinMaxLabel(scroll_bar_current_range);
        recalculate_ = false;
    }

    std::pair<double, double> MACD::GetMinMaxLabelValue() const
    {
        return min_max_label_;
    }

    void MACD::Draw(juce::Graphics& g,
                    juce::Rectangle<int> chart_bounds,
                    juce::Rectangle<int> label_bounds,
                    int bar_width,
                    const juce::Range<int>& scroll_bar_current_range,
                    const std::pair<double, double>& min_max_label)
    {
        DrawXGridAndLabel(g, chart_bounds, label_bounds, min_max_label);

        const auto max_period = std::max(ema_long_period_, macd_period_);
        DrawLine(g, chart_bounds, bar_width, scroll_bar_current_range, min_max_label, juce::Colours::yellow, dif_array_, max_period);
        DrawLine(g, chart_bounds, bar_width, scroll_bar_current_range, min_max_label, juce::Colours::orange, macd_array_, max_period);
        DrawBar(g, chart_bounds, bar_width, scroll_bar_current_range, min_max_label, osc_array_, max_period);
    }

    void MACD::DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index)
    {
        if (k_index == -1)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);
        const auto font = GetWatchToolMessageFont();
        g.setFont(font);

        juce::String dif_message("DIF" + juce::String(ema_short_period_) + "-" + juce::String(ema_long_period_) + " ");
        juce::String macd_message("MACD" + juce::String(macd_period_) + " ");
        juce::String osc_message("OSC ");

        const auto max_period = std::max(ema_long_period_, macd_period_);
        if (k_index >= max_period)
        {
            dif_message += juce::String::formatted("%.2f", dif_array_[k_index]);
            macd_message += juce::String::formatted("%.2f", macd_array_[k_index]);
            osc_message += juce::String::formatted("%.2f", osc_array_[k_index]);
        }
        else
        {
            dif_message += "--";
            macd_message += "--";
            osc_message += "--";
        }

        g.setColour(juce::Colours::yellow);
        chart_bounds.removeFromLeft(1);
        g.drawText(dif_message, chart_bounds.removeFromLeft(font.getStringWidth(dif_message)), juce::Justification::topLeft, false);

        g.setColour(juce::Colours::orange);
        chart_bounds.removeFromLeft(10);
        g.drawText(macd_message, chart_bounds.removeFromLeft(font.getStringWidth(macd_message)), juce::Justification::topLeft, false);

        g.setColour(juce::Colours::white);
        chart_bounds.removeFromLeft(10);
        g.drawText(osc_message, chart_bounds.removeFromLeft(font.getStringWidth(osc_message)), juce::Justification::topLeft, false);
    }

    std::pair<double, double> MACD::CalculateMinMaxLabel(const juce::Range<int>& scroll_bar_current_range) const
    {
        std::pair<double, double> min_max_label;

        const auto max_period = std::max(ema_long_period_, macd_period_);
        auto [min, max] = std::minmax_element(dif_array_.begin() + std::max(scroll_bar_current_range.getStart(), max_period),
                                              dif_array_.begin() + std::max(scroll_bar_current_range.getEnd(), max_period));

        if (min != dif_array_.end() && max != dif_array_.end())
        {
            min_max_label = std::make_pair(*min, *max);
        }

        std::tie(min, max) = std::minmax_element(macd_array_.begin() + std::max(scroll_bar_current_range.getStart(), max_period),
                                                 macd_array_.begin() + std::max(scroll_bar_current_range.getEnd(), max_period));

        if (min != macd_array_.end() && max != macd_array_.end())
        {
            min_max_label.first = std::min(*min, min_max_label.first);
            min_max_label.second = std::max(*max, min_max_label.second);
        }

        std::tie(min, max) = std::minmax_element(osc_array_.begin() + std::max(scroll_bar_current_range.getStart(), max_period),
                                                 osc_array_.begin() + std::max(scroll_bar_current_range.getEnd(), max_period));

        if (min != osc_array_.end() && max != osc_array_.end())
        {
            min_max_label.first = std::min(*min, min_max_label.first);
            min_max_label.second = std::max(*max, min_max_label.second);
        }

        const auto tweak_max = std::max<double>(std::abs(std::floor(min_max_label.first)), std::abs(std::ceil(min_max_label.second)));
        return std::make_pair(-tweak_max, tweak_max);
    }

    void MACD::DrawLine(juce::Graphics& g,
                        juce::Rectangle<int> chart_bounds,
                        int bar_width,
                        const juce::Range<int>& scroll_bar_current_range,
                        const std::pair<double, double>& min_max_label,
                        const juce::Colour& line_color,
                        const std::vector<double>& data_array,
                        int period)
    {
        if (min_max_label.first == min_max_label.second)
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

            if (std::distance(data_array.cbegin(), it) < period)
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

    void MACD::DrawBar(juce::Graphics& g,
                       juce::Rectangle<int> chart_bounds,
                       int bar_width,
                       const juce::Range<int>& scroll_bar_current_range,
                       const std::pair<double, double>& min_max_label,
                       const std::vector<double>& data_array,
                       int period)
    {
        if (min_max_label.first == min_max_label.second)
        {
            return;
        }

        juce::Graphics::ScopedSaveState raii(g);
        g.setColour(juce::Colours::white);

        const auto ratio = chart_bounds.getHeight() / (min_max_label.second - min_max_label.first);
        const auto begin = data_array.cbegin() + scroll_bar_current_range.getStart();
        const auto end = data_array.cbegin() + scroll_bar_current_range.getEnd();

        for (auto it = begin; it != end; ++it)
        {
            chart_bounds.removeFromLeft(kBarGap);
            const auto bar_bounds = chart_bounds.removeFromLeft(bar_width);

            if (std::distance(data_array.cbegin(), it) < period)
            {
                continue;
            }

            const auto value = *it;
            g.setColour(value > 0 ? juce::Colours::red : (value < 0 ? juce::Colours::green : juce::Colours::white));
            if (value > 0)
            {
                const auto height = juce::roundToInt(std::max(value * ratio, 1.0));
                g.fillRect(bar_bounds.getX(),
                           bar_bounds.getCentreY() - height,
                           bar_bounds.getWidth(),
                           height);
            }
            else if (value < 0)
            {
                g.fillRect(bar_bounds.getX(),
                           bar_bounds.getCentreY(),
                           bar_bounds.getWidth(),
                           juce::roundToInt(std::max(abs(value) * ratio, 1.0)));
            }
            else
            {
                g.drawHorizontalLine(bar_bounds.getCentreY(), bar_bounds.getX(), bar_bounds.getRight());
            }
        }
    }

    void MACD::DrawXGridAndLabel(juce::Graphics& g,
                                 juce::Rectangle<int> chart_bounds,
                                 juce::Rectangle<int> label_bounds,
                                 const std::pair<double, double>& min_max_label)
    {
        juce::Graphics::ScopedSaveState raii(g);

        const std::array<double, 3> label_values = { min_max_label.first / 2, 0, min_max_label.second / 2 };
        const auto min_max_range = min_max_label.second - min_max_label.first;

        g.setColour(juce::Colours::grey);
        for (const auto& label_value : label_values)
        {
            g.drawHorizontalLine(juce::roundToInt(chart_bounds.getY() + chart_bounds.getHeight() * (min_max_label.second - label_value) / min_max_range),
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
                                              label_bounds.getY() + label_bounds.getHeight() * (min_max_label.second - label_value) / min_max_range - font_height / 2,
                                              label_bounds.getWidth(),
                                              font_height),
                       juce::Justification::centredLeft,
                       false);
        }
    }

    std::vector<double> MACD::EMA(const std::vector<double>& value_array, int period)
    {
        const auto size = value_array.size();
        if (period > size)
        {
            return {};
        }

        std::vector<double> ema;
        ema.reserve(size);

        for (int i = 0; i < period - 1; ++i)
        {
            ema.push_back(0);
        }

        ema.push_back(std::accumulate(value_array.begin(), value_array.begin() + period, 0.0) / period);

        const auto alpha = 2.0 / (period + 1);
        const auto beta = 1 - alpha;
        for (int i = period; i < size; ++i)
        {
            ema.push_back(ema[i - 1] * beta + value_array[i] * alpha);
        }

        return ema;
    }

    std::vector<double> MACD::DIF(const std::vector<double>& ema_short_array,
                                  int ema_short_period,
                                  const std::vector<double>& ema_long_array,
                                  int ema_long_period)
    {
        const auto size = ema_short_array.size();
        std::vector<double> dif_array;
        dif_array.reserve(size);

        for (int i = 0; i < ema_long_period; ++i)
        {
            dif_array.push_back(0);
        }

        for (int i = ema_long_period; i < size; ++i)
        {
            dif_array.push_back(ema_short_array[i] - ema_long_array[i]);
        }

        return dif_array;
    }

    std::vector<double> MACD::OSC(const std::vector<double>& dif_array,
                                  int ema_long_period,
                                  const std::vector<double>& macd_array,
                                  int macd_period)
    {
        const auto size = dif_array.size();
        std::vector<double> osc_array;
        osc_array.reserve(size);

        const auto max_period = std::max(ema_long_period, macd_period);
        for (int i = 0; i < max_period; ++i)
        {
            osc_array.push_back(0);
        }

        for (int i = max_period; i < size; ++i)
        {
            osc_array.push_back(dif_array[i] - macd_array[i]);
        }

        return osc_array;
    }
}
