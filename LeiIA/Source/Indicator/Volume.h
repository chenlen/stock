// © 2023 Lei Cheng

#pragma once

#include "Indicator.h"
#include "Data/DataCenter.h"

namespace lei
{
    class Volume : public Indicator
    {
    public:
        explicit Volume(const std::function<const KArray& ()>& GetKArray);
        ~Volume() override;

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
        static void DrawXGridAndLabel(juce::Graphics& g,
                                      juce::Rectangle<int> chart_bounds,
                                      juce::Rectangle<int> label_bounds,
                                      unsigned long long max_volume);

        static void DrawVolumeBar(juce::Graphics& g,
                                  juce::Rectangle<int> chart_bounds,
                                  const VolumeArray& volume_array,
                                  const CloseArray& close_array,
                                  const juce::Range<int>& scroll_bar_current_range,
                                  unsigned long long max_volume,
                                  int bar_width);

    private:
        std::function<const KArray& ()> GetKArray_;
        std::pair<double, double> min_max_label_;
    };
}
