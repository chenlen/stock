// © 2023 Lei Cheng

#pragma once

#include "Indicator.h"
#include "Data/DataCenter.h"

namespace lei
{
    class MACD : public Indicator
    {
    public:
        MACD(const std::function<const KArray& ()>& GetKArray, int ema_short_period, int ema_long_period, int macd_period);
        ~MACD() override;

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
        static void DrawLine(juce::Graphics& g,
                             juce::Rectangle<int> chart_bounds,
                             int bar_width,
                             const juce::Range<int>& scroll_bar_current_range,
                             const std::pair<double, double>& min_max_label,
                             const juce::Colour& line_color,
                             const std::vector<double>& data_array,
                             int period);

        static void DrawBar(juce::Graphics& g,
                            juce::Rectangle<int> chart_bounds,
                            int bar_width,
                            const juce::Range<int>& scroll_bar_current_range,
                            const std::pair<double, double>& min_max_label,
                            const std::vector<double>& data_array,
                            int period);

        static void DrawXGridAndLabel(juce::Graphics& g,
                                      juce::Rectangle<int> chart_bounds,
                                      juce::Rectangle<int> label_bounds,
                                      const std::pair<double, double>& min_max_label);

        static std::vector<double> EMA(const std::vector<double>& value_array, int period);
        static std::vector<double> DIF(const std::vector<double>& ema_short_array,
                                       int ema_short_period,
                                       const std::vector<double>& ema_long_array,
                                       int ema_long_period);

        static std::vector<double> OSC(const std::vector<double>& dif_array,
                                       int ema_long_period,
                                       const std::vector<double>& macd_array,
                                       int macd_period);

    private:
        std::function<const KArray& ()> GetKArray_;
        std::pair<double, double> min_max_label_;
        int ema_short_period_;
        int ema_long_period_;
        int macd_period_;
        std::vector<double> dif_array_;
        std::vector<double> macd_array_;
        std::vector<double> osc_array_;
        bool recalculate_ = true;
    };
}
