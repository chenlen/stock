// © 2023 Lei Cheng

#pragma once

#include "Indicator.h"
#include "Data/DataCenter.h"

namespace lei
{
    class MA : public Indicator
    {
    public:
        MA(const std::function<const KArray& ()>& GetKArray, int period, const juce::Colour& color);
        ~MA() override;

    public:
        IndicatorType GetIndicatorType() const override;

        void StockChanged() override;

        void Calculate(const juce::Range<int>& scroll_bar_current_range) override;

        std::pair<double, double> GetMinMaxLabelValue() const override;

        void Draw(juce::Graphics& g,
                  juce::Rectangle<int> chart_bounds,
                  juce::Rectangle<int> label_bounds,
                  int bar_width,
                  const juce::Range<int>& scroll_bar_current_range,
                  const std::pair<double, double>& min_max_label) override;

        void DrawWatchToolMessage(juce::Graphics& g, juce::Rectangle<int> chart_bounds, int k_index) override;

    private:
        std::pair<double, double> CalculateMinMaxLabel(const juce::Range<int>& scroll_bar_current_range) const;

    private:
        std::function<const KArray& ()> GetKArray_;
        std::pair<double, double> min_max_label_;
        int period_;
        std::vector<double> ma_array_;
        bool recalculate_ = true;
        juce::Colour color_;
    };
}
