#pragma once

#include <JuceHeader.h>
#include "Indicator/Indicator.h"
#include "KChart/KChart.h"
#include "Key.h"
#include "Layout.h"
#include "Tool/Tool.h"
#include "Tool/ToolType.h"
#include "WatchTool/WatchTool.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/

class MainComponent : public juce::Component, public juce::ScrollBar::Listener, public juce::Button::Listener, public juce::TextEditor::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

public:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void scrollBarMoved(juce::ScrollBar* scroll_bar_that_has_moved, double new_range_start) override;
    void buttonClicked(juce::Button* button) override;
    void textEditorReturnKeyPressed(juce::TextEditor& text_editor) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;

public:
    const lei::KArray& GetKArray() const;

private:
    void DrawKChart(juce::Graphics& g);
    void DrawSubsidiaryCharts(juce::Graphics& g);
    void DrawWatchToolMessage(juce::Graphics& g);
    void HandleZoomChanged();
    void StockChanged(const std::string& stock_id);
    void DataFrequencyChanged(lei::DataFrequency frequency);
    void ToolChanged(lei::ToolType tool_type);
    int CalculateScreenKSize() const;
    int GetKIndexRestrictInBounds(const juce::Point<int>& pt) const;
    int GetKCentreXRestrictInBounds(const juce::Point<int>& pt) const;
    int GetChartIndex(const juce::Point<int>& pt) const;

    juce::Rectangle<int> GetChartBounds(const juce::Point<int>& pt) const;
    juce::Rectangle<int> GetChartBounds(int chart_index) const;

    juce::Rectangle<int> GetPriceLabelBounds(const juce::Point<int>& pt) const;

    std::pair<double, double> GetChartMinMaxLabel(const juce::Point<int>& pt) const;
    std::pair<double, double> GetChartMinMaxLabel(int chart_index) const;

    void RegisterEraseButton(const std::shared_ptr<juce::Button>& erase_button);
    void UnregisterEraseButton(const std::shared_ptr<juce::Button>& erase_button);
    std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>> GetTools(const std::string& stock_id, lei::DataFrequency data_frequency) const;

private:
    enum
    {
        kMainIndicatorSize = 3,
        kSubsidiaryChartSize = 3
    };

    juce::TextEditor stock_search_bar_;
    juce::TextButton data_frequency_button_;
    juce::TextButton tool_button_;

    juce::Rectangle<int> toolbar_bounds_;
    juce::Rectangle<int> header_bounds_;
    juce::Rectangle<int> chart_bounds_;
    juce::Rectangle<int> time_label_bounds_;

    juce::Rectangle<int> k_chart_bounds_;
    juce::Rectangle<int> k_price_label_bounds_;

    std::deque<juce::Rectangle<int>> subsidiary_charts_bounds_;
    std::deque<juce::Rectangle<int>> subsidiary_labels_bounds_;

    juce::ScrollBar chart_scroll_bar_;

    juce::ImageButton zoom_in_button_;
    juce::ImageButton zoom_out_button_;
    juce::ImageButton zoom_reset_button_;
    int bar_width_ = lei::kDefaultBarWidth;

    std::string stock_id_;
    lei::DataFrequency data_frequency_;

    lei::ToolType tool_type_;
    std::unordered_map<std::pair<std::string, lei::DataFrequency>, std::unordered_map<juce::Uuid, std::shared_ptr<lei::Tool>>> tools_;
    std::unique_ptr<lei::Tool> tool_;

    int k_index_ = 0;
    lei::WatchTool watch_tool_;

    std::pair<double, double> k_chart_min_max_label_;

    std::array<std::unique_ptr<lei::Indicator>, kMainIndicatorSize> main_indicators_;
    std::array<std::unique_ptr<lei::Indicator>, kSubsidiaryChartSize> subsidiary_indicators_;

    std::unique_ptr<lei::KChart> k_chart_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
