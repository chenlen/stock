// © 2023 Lei Cheng

#pragma once

#include "Indicator.h"
#include "Data/DataCenter.h"

namespace lei
{
    class KD : public Indicator
    {
    public:
        KD(const std::function<const KArray& ()>& GetKArray, int period, int rsv_weight, int k_weight);
        ~KD() override;

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
        static void DrawLine(juce::Graphics& g,
                             juce::Rectangle<int> chart_bounds,
                             int bar_width,
                             const juce::Range<int>& scroll_bar_current_range,
                             const std::pair<double, double>& min_max_label,
                             const juce::Colour& line_color,
                             const std::vector<double>& data_array,
                             int period);

        static void DrawXGridAndLabel(juce::Graphics& g, juce::Rectangle<int> chart_bounds, juce::Rectangle<int> label_bounds);

    private:
        std::function<const KArray& ()> GetKArray_;
        int period_;
        int rsv_weight_;
        int k_weight_;
        std::vector<double> rsv_array_;
        std::vector<double> k_array_;
        std::vector<double> d_array_;
        bool recalculate_ = true;
    };
}
