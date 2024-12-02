// © 2023 Lei Cheng

#include "MainComponent.h"
#include "DrawUtility.h"
#include "Indicator/K.h"
#include "Indicator/KD.h"
#include "Indicator/MA.h"
#include "Indicator/MACD.h"
#include "Indicator/Volume.h"
#include "Tool/ToolFactory.h"

namespace
{
    juce::Range<int> ToInt(const juce::Range<double>& range)
    {
        return { juce::roundToInt(range.getStart()), juce::roundToInt(range.getEnd()) };
    }
}

MainComponent::MainComponent() :
    chart_scroll_bar_(false),
    stock_id_("2330.tw"),
    data_frequency_(lei::DataFrequency::kDay),
    tool_type_(lei::ToolType::kNone),
    tool_(lei::ToolFactory::GetTool(lei::ToolType::kNone,
                                    this,
                                    std::bind(&MainComponent::GetKArray, this),
                                    GetTools(stock_id_, data_frequency_),
                                    std::bind(&MainComponent::RegisterEraseButton, this, std::placeholders::_1),
                                    std::bind(&MainComponent::UnregisterEraseButton, this, std::placeholders::_1))),
    watch_tool_(this, std::bind(&MainComponent::GetKArray, this)),
    main_indicators_{ std::make_unique<lei::K>(std::bind(&MainComponent::GetKArray, this)),
                      std::make_unique<lei::MA>(std::bind(&MainComponent::GetKArray, this), 5, juce::Colours::yellow),
                      std::make_unique<lei::MA>(std::bind(&MainComponent::GetKArray, this), 22, juce::Colours::orange) },
    subsidiary_indicators_{ std::make_unique<lei::Volume>(std::bind(&MainComponent::GetKArray, this)),
                            std::make_unique<lei::KD>(std::bind(&MainComponent::GetKArray, this), 9, 3, 3),
                            std::make_unique<lei::MACD>(std::bind(&MainComponent::GetKArray, this), 12, 26, 9) },
    k_chart_(std::make_unique<lei::KChart>())
{
    stock_search_bar_.setFont(lei::GeStockSearchBarFont());
    stock_search_bar_.setTextToShowWhenEmpty(juce::translate("stock id"), juce::Colours::grey);
    stock_search_bar_.addListener(this);
    addAndMakeVisible(stock_search_bar_);

    data_frequency_button_.onClick = [this]
        {
            juce::PopupMenu menu;
            menu.addItem(juce::translate("1 min frequency"),
                         true,
                         data_frequency_ == lei::DataFrequency::k1Min,
                         std::bind(&MainComponent::DataFrequencyChanged, this, lei::DataFrequency::k1Min));

            menu.addItem(juce::translate("day frequency"),
                         true,
                         data_frequency_ == lei::DataFrequency::kDay,
                         std::bind(&MainComponent::DataFrequencyChanged, this, lei::DataFrequency::kDay));

            menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(data_frequency_button_));
        };

    data_frequency_button_.setButtonText(juce::translate("data frequency"));
    addAndMakeVisible(data_frequency_button_);

    tool_button_.onClick = [this]
        {
            juce::PopupMenu menu;
            menu.addItem(juce::translate("none tool"),
                         true,
                         tool_type_ == lei::ToolType::kNone,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kNone));

            menu.addItem(juce::translate("vertical line tool"),
                         true,
                         tool_type_ == lei::ToolType::kVerticalLine,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kVerticalLine));

            menu.addItem(juce::translate("horizontal line tool"),
                         true,
                         tool_type_ == lei::ToolType::kHorizontalLine,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kHorizontalLine));

            menu.addItem(juce::translate("trendline tool"),
                         true,
                         tool_type_ == lei::ToolType::kTrendline,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kTrendline));

            menu.addItem(juce::translate("parallel lines tool"),
                         true,
                         tool_type_ == lei::ToolType::kParallelLines,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kParallelLines));

            menu.addItem(juce::translate("line tool"),
                         true,
                         tool_type_ == lei::ToolType::kLine,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kLine));

            menu.addItem(juce::translate("time fibonacci sequence tool"),
                         true,
                         tool_type_ == lei::ToolType::kTimeFibonacciSequence,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kTimeFibonacciSequence));

            menu.addItem(juce::translate("erase tool"),
                         true,
                         tool_type_ == lei::ToolType::kErase,
                         std::bind(&MainComponent::ToolChanged, this, lei::ToolType::kErase));

            menu.addItem(juce::translate("erase all"), true, false, [this]()
                         {
                             const auto pos = tools_.find({ stock_id_, data_frequency_ });
                             if (pos != tools_.end())
                             {
                                 pos->second.clear();
                                 repaint();
                             }

                             ToolChanged(lei::ToolType::kNone);
                         });

            menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(tool_button_));
        };

    tool_button_.setButtonText(juce::translate("tools"));
    addAndMakeVisible(tool_button_);

    chart_scroll_bar_.setRangeLimits(0, std::get<0>(lei::GetKDataCenter().GetKData(stock_id_, data_frequency_)).size());
    chart_scroll_bar_.setSingleStepSize(1);
    chart_scroll_bar_.scrollToBottom();
    chart_scroll_bar_.addListener(this);
    addAndMakeVisible(chart_scroll_bar_);

    const auto zoom_in_image = juce::ImageCache::getFromFile(juce::File::getCurrentWorkingDirectory().getChildFile("assets").getChildFile("icon").getChildFile("zoom_in.png"));
    zoom_in_button_.setImages(false, true, true,
                              zoom_in_image, 1, juce::Colours::white,
                              zoom_in_image, 1, juce::Colours::white,
                              zoom_in_image, 1, juce::Colours::grey);

    zoom_in_button_.addListener(this);
    addAndMakeVisible(zoom_in_button_);

    const auto zoom_out_image = juce::ImageCache::getFromFile(juce::File::getCurrentWorkingDirectory().getChildFile("assets").getChildFile("icon").getChildFile("zoom_out.png"));
    zoom_out_button_.setImages(false, true, true,
                               zoom_out_image, 1, juce::Colours::white,
                               zoom_out_image, 1, juce::Colours::white,
                               zoom_out_image, 1, juce::Colours::grey);

    zoom_out_button_.addListener(this);
    addAndMakeVisible(zoom_out_button_);

    const auto restore_image = juce::ImageCache::getFromFile(juce::File::getCurrentWorkingDirectory().getChildFile("assets").getChildFile("icon").getChildFile("restore.png"));
    zoom_reset_button_.setImages(false, true, true,
                                 restore_image, 1, juce::Colours::white,
                                 restore_image, 1, juce::Colours::white,
                                 restore_image, 1, juce::Colours::grey);

    zoom_reset_button_.addListener(this);
    addAndMakeVisible(zoom_reset_button_);

    setSize(1850, 900);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    DrawKChart(g);
    DrawSubsidiaryCharts(g);

    const auto pos = tools_.find({ stock_id_, data_frequency_ });
    if (pos != tools_.end())
    {
        for (const auto& tool : pos->second)
        {
            const auto chart_index = tool.second->GetChartIndex();
            tool.second->Paint(g);
        }
    }

    const auto chart_index = tool_->GetChartIndex();
    tool_->Paint(g);

    watch_tool_.Paint(g);
    DrawWatchToolMessage(g);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();

    toolbar_bounds_ = bounds.removeFromTop(lei::kToolbarHeight);

    juce::FlexBox box;
    box.flexDirection = juce::FlexBox::Direction::row;
    box.alignItems = juce::FlexBox::AlignItems::center;
    box.items = { juce::FlexItem(lei::kStockSearchBarWidth, lei::kStockSearchBarHeight, stock_search_bar_).withMargin({lei::kToolGap}),
        juce::FlexItem(lei::kDataFrequencyButtonWidth, lei::kDataFrequencyButtonHeight, data_frequency_button_).withMargin({lei::kToolGap}),
        juce::FlexItem(lei::kToolButtonWidth, lei::kToolButtonHeight, tool_button_).withMargin({lei::kToolGap}) };

    box.performLayout(toolbar_bounds_);

    auto widgets_bounds = bounds.removeFromBottom(lei::kWidgetsHeight);
    zoom_reset_button_.setBounds(widgets_bounds.removeFromRight(lei::kZoomButtonWidth));
    zoom_out_button_.setBounds(widgets_bounds.removeFromRight(lei::kZoomButtonWidth));
    zoom_in_button_.setBounds(widgets_bounds.removeFromRight(lei::kZoomButtonWidth));

    chart_scroll_bar_.setBounds(widgets_bounds);

    header_bounds_ = bounds.removeFromTop(lei::kHeaderHeight);
    time_label_bounds_ = bounds.removeFromBottom(lei::kTimeLabelHeight);

    chart_bounds_ = bounds;
    chart_bounds_.removeFromLeft(lei::kLeftLabelWidth);
    chart_bounds_.removeFromRight(lei::kRightLabelWidth);

    const auto chart_height = bounds.getHeight();

    subsidiary_charts_bounds_.clear();
    subsidiary_labels_bounds_.clear();
    for (int i = 0; i < kSubsidiaryChartSize; ++i)
    {
        auto chart_bounds_ = bounds.removeFromBottom(chart_height * lei::kSubsidiaryChartHeightPercentage / 100.0);
        chart_bounds_.removeFromTop(lei::kChartGap);
        chart_bounds_.removeFromLeft(lei::kLeftLabelWidth);
        subsidiary_labels_bounds_.push_front(chart_bounds_.removeFromRight(lei::kRightLabelWidth));
        subsidiary_charts_bounds_.push_front(chart_bounds_);
    }

    k_chart_bounds_ = bounds;
    k_chart_bounds_.removeFromLeft(lei::kLeftLabelWidth);
    k_price_label_bounds_ = k_chart_bounds_.removeFromRight(lei::kRightLabelWidth);

    const auto screen_k_size = CalculateScreenKSize();
    chart_scroll_bar_.setCurrentRange(chart_scroll_bar_.getCurrentRange().getEnd() - screen_k_size, screen_k_size);
    HandleZoomChanged();
}

void MainComponent::scrollBarMoved(juce::ScrollBar* scroll_bar_that_has_moved, double new_range_start)
{
    if (scroll_bar_that_has_moved == &chart_scroll_bar_)
    {
        HandleZoomChanged();
        repaint();
    }
}

void MainComponent::buttonClicked(juce::Button* button)
{
    if (button == &zoom_in_button_)
    {
        int kMaxBarWidth = k_chart_bounds_.reduced(lei::kChartBorderThickness, 0).getWidth() - lei::kBarGap * 2;
        if (kMaxBarWidth % 2 == 0) // Must odd
        {
            --kMaxBarWidth;
        }

        bar_width_ += 2;
        bar_width_ = std::min(bar_width_, kMaxBarWidth);
        const auto screen_k_size = CalculateScreenKSize();
        chart_scroll_bar_.setCurrentRange(chart_scroll_bar_.getCurrentRange().getEnd() - screen_k_size, screen_k_size);
        HandleZoomChanged();
        repaint();
    }
    else if (button == &zoom_out_button_)
    {
        const int kMinBarWidth = 3;
        bar_width_ -= 2;
        bar_width_ = std::max(bar_width_, kMinBarWidth);
        const auto screen_k_size = CalculateScreenKSize();
        chart_scroll_bar_.setCurrentRange(chart_scroll_bar_.getCurrentRange().getEnd() - screen_k_size, screen_k_size);
        HandleZoomChanged();
        repaint();
    }
    else if (button == &zoom_reset_button_)
    {
        bar_width_ = lei::kDefaultBarWidth;
        const auto screen_k_size = CalculateScreenKSize();
        chart_scroll_bar_.setCurrentRange(chart_scroll_bar_.getCurrentRange().getEnd() - screen_k_size, screen_k_size);
        HandleZoomChanged();
        repaint();
    }
    else if (button->getName().equalsIgnoreCase("erase tool"))
    {
        const auto pos = tools_.find({ stock_id_, data_frequency_ });
        if (pos != tools_.end())
        {
            pos->second.erase(button->getButtonText());
            repaint();
        }
    }
}

void MainComponent::textEditorReturnKeyPressed(juce::TextEditor& text_editor)
{
    if (&text_editor == &stock_search_bar_)
    {
        auto new_stock_id = text_editor.getText();
        if (!new_stock_id.getLastCharacters(3).equalsIgnoreCase(".tw"))
        {
            new_stock_id += ".tw";
        }

        if (!new_stock_id.equalsIgnoreCase(stock_id_.c_str()))
        {
            StockChanged(new_stock_id.toStdString());
            repaint();
        }
    }
}

void MainComponent::mouseMove(const juce::MouseEvent& event)
{
    const auto pt = event.position.roundToInt();
    k_index_ = GetKIndexRestrictInBounds(pt);
    watch_tool_.MouseEvent(pt,
                           k_index_,
                           chart_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                           GetPriceLabelBounds(pt).reduced(lei::kChartBorderThickness),
                           time_label_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartMinMaxLabel(pt),
                           data_frequency_,
                           GetKCentreXRestrictInBounds(pt));
}

void MainComponent::mouseEnter(const juce::MouseEvent& event)
{
    const auto pt = event.position.roundToInt();
    k_index_ = GetKIndexRestrictInBounds(pt);
    watch_tool_.MouseEvent(pt,
                           k_index_,
                           chart_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                           GetPriceLabelBounds(pt).reduced(lei::kChartBorderThickness),
                           time_label_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartMinMaxLabel(pt),
                           data_frequency_,
                           GetKCentreXRestrictInBounds(pt));
}

void MainComponent::mouseExit(const juce::MouseEvent& event)
{
    const auto pt = event.position.roundToInt();
    k_index_ = GetKIndexRestrictInBounds(pt);
    watch_tool_.MouseEvent(pt,
                           k_index_,
                           chart_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                           GetPriceLabelBounds(pt).reduced(lei::kChartBorderThickness),
                           time_label_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartMinMaxLabel(pt),
                           data_frequency_,
                           GetKCentreXRestrictInBounds(pt));
}

void MainComponent::mouseDown(const juce::MouseEvent& event)
{
    const auto pt = event.position.roundToInt();
    k_index_ = GetKIndexRestrictInBounds(pt);
    watch_tool_.MouseEvent(pt,
                           k_index_,
                           chart_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                           GetPriceLabelBounds(pt).reduced(lei::kChartBorderThickness),
                           time_label_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartMinMaxLabel(pt),
                           data_frequency_,
                           GetKCentreXRestrictInBounds(pt));

    tool_->ToolBegin(pt,
                     IsAcrossCharts(tool_->GetToolType()) ? chart_bounds_.reduced(lei::kChartBorderThickness) : GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                     GetChartMinMaxLabel(pt),
                     ToInt(chart_scroll_bar_.getCurrentRange()),
                     bar_width_,
                     GetChartIndex(pt));
}

void MainComponent::mouseDrag(const juce::MouseEvent& event)
{
    const auto pt = event.position.roundToInt();
    k_index_ = GetKIndexRestrictInBounds(pt);
    watch_tool_.MouseEvent(pt,
                           k_index_,
                           chart_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                           GetPriceLabelBounds(pt).reduced(lei::kChartBorderThickness),
                           time_label_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartMinMaxLabel(pt),
                           data_frequency_,
                           GetKCentreXRestrictInBounds(pt));

    tool_->ToolProcess(pt);
}

void MainComponent::mouseUp(const juce::MouseEvent& event)
{
    const auto pt = event.position.roundToInt();
    k_index_ = GetKIndexRestrictInBounds(pt);
    watch_tool_.MouseEvent(pt,
                           k_index_,
                           chart_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartBounds(pt).reduced(lei::kChartBorderThickness),
                           GetPriceLabelBounds(pt).reduced(lei::kChartBorderThickness),
                           time_label_bounds_.reduced(lei::kChartBorderThickness),
                           GetChartMinMaxLabel(pt),
                           data_frequency_,
                           GetKCentreXRestrictInBounds(pt));

    tool_->ToolEnd(pt);

    if (tool_->IsToolFinished())
    {
        const std::pair<std::string, lei::DataFrequency> key = { stock_id_, data_frequency_ };
        const auto pos = tools_.find(key);
        if (pos != tools_.end())
        {
            pos->second.emplace(tool_->GetKey(), std::move(tool_));
        }
        else
        {
            std::unordered_map<juce::Uuid, std::shared_ptr<lei::Tool>> tools;
            tools.emplace(tool_->GetKey(), std::move(tool_));
            tools_.emplace(key, std::move(tools));
        }

        tool_ = lei::ToolFactory::GetTool(tool_type_,
                                          this,
                                          std::bind(&MainComponent::GetKArray, this),
                                          GetTools(stock_id_, data_frequency_),
                                          std::bind(&MainComponent::RegisterEraseButton, this, std::placeholders::_1),
                                          std::bind(&MainComponent::UnregisterEraseButton, this, std::placeholders::_1));
    }
}

const lei::KArray& MainComponent::GetKArray() const
{
    return lei::GetKDataCenter().GetKData(stock_id_, data_frequency_);
}

void MainComponent::DrawKChart(juce::Graphics& g)
{
    const auto& k_array = lei::GetKDataCenter().GetKData(stock_id_, data_frequency_);
    k_chart_->DrawHeader(g,
                         header_bounds_.reduced(lei::kChartBorderThickness),
                         stock_id_,
                         std::get<0>(k_array),
                         std::get<1>(k_array),
                         std::get<2>(k_array),
                         std::get<3>(k_array),
                         std::get<4>(k_array),
                         std::get<5>(k_array),
                         lei::GetHeaderFont(),
                         data_frequency_);

    k_chart_->DrawBounds(g, k_chart_bounds_);

    const auto scroll_bar_current_range = ToInt(chart_scroll_bar_.getCurrentRange());
    k_chart_->DrawTimeGrid(g,
                           k_chart_bounds_.reduced(lei::kChartBorderThickness),
                           std::get<0>(k_array),
                           scroll_bar_current_range,
                           bar_width_,
                           data_frequency_);

    k_chart_->DrawTimeLabel(g,
                            k_chart_bounds_.reduced(lei::kChartBorderThickness),
                            time_label_bounds_.reduced(lei::kChartBorderThickness),
                            std::get<0>(k_array),
                            scroll_bar_current_range,
                            bar_width_,
                            data_frequency_);

    for (const auto& indicator : main_indicators_)
    {
        indicator->Draw(g,
                        k_chart_bounds_.reduced(lei::kChartBorderThickness),
                        k_price_label_bounds_.reduced(lei::kChartBorderThickness),
                        bar_width_,
                        scroll_bar_current_range,
                        k_chart_min_max_label_);
    }
}

void MainComponent::DrawSubsidiaryCharts(juce::Graphics& g)
{
    const auto scroll_bar_current_range = ToInt(chart_scroll_bar_.getCurrentRange());
    for (int i = 0; i < kSubsidiaryChartSize; ++i)
    {
        k_chart_->DrawBounds(g, subsidiary_charts_bounds_[i]);
        k_chart_->DrawTimeGrid(g,
                               subsidiary_charts_bounds_[i].reduced(lei::kChartBorderThickness),
                               std::get<0>(lei::GetKDataCenter().GetKData(stock_id_, data_frequency_)),
                               scroll_bar_current_range,
                               bar_width_,
                               data_frequency_);

        subsidiary_indicators_[i]->Draw(g,
                                        subsidiary_charts_bounds_[i].reduced(lei::kChartBorderThickness),
                                        subsidiary_labels_bounds_[i].reduced(lei::kChartBorderThickness),
                                        bar_width_,
                                        scroll_bar_current_range,
                                        subsidiary_indicators_[i]->GetMinMaxLabelValue());
    }
}

void MainComponent::DrawWatchToolMessage(juce::Graphics& g)
{
    auto k_chart_bounds_exclude_border = k_chart_bounds_.reduced(lei::kChartBorderThickness);
    const auto font = lei::GetWatchToolMessageFont();
    for (const auto& indicator : main_indicators_)
    {
        indicator->DrawWatchToolMessage(g, k_chart_bounds_exclude_border.removeFromTop(font.getHeight()), k_index_);
    }

    for (int i = 0; i < kSubsidiaryChartSize; ++i)
    {
        subsidiary_indicators_[i]->DrawWatchToolMessage(g, subsidiary_charts_bounds_[i], k_index_);
    }
}

void MainComponent::HandleZoomChanged()
{
    k_chart_min_max_label_.first = std::numeric_limits<double>::max();
    k_chart_min_max_label_.second = std::numeric_limits<double>::min();

    const auto scroll_bar_current_range = ToInt(chart_scroll_bar_.getCurrentRange());

    for (const auto& indicator : main_indicators_)
    {
        indicator->Calculate(scroll_bar_current_range);
        const auto min_max_label = indicator->GetMinMaxLabelValue();
        k_chart_min_max_label_.first = std::min(min_max_label.first, k_chart_min_max_label_.first);
        k_chart_min_max_label_.second = std::max(min_max_label.second, k_chart_min_max_label_.second);
    }

    for (const auto& indicator : subsidiary_indicators_)
    {
        indicator->Calculate(scroll_bar_current_range);
    }

    const auto pos = tools_.find({ stock_id_, data_frequency_ });
    if (pos != tools_.end())
    {
        for (auto& it : pos->second)
        {
            const auto chart_index = it.second->GetChartIndex();
            it.second->ZoomChanged(IsAcrossCharts(it.second->GetToolType()) ? chart_bounds_.reduced(lei::kChartBorderThickness) : GetChartBounds(chart_index).reduced(lei::kChartBorderThickness),
                                   GetChartMinMaxLabel(chart_index),
                                   ToInt(chart_scroll_bar_.getCurrentRange()),
                                   bar_width_);
        }
    }

    if (tool_type_ == lei::ToolType::kErase)
    {
        tool_->ZoomChanged({}, {}, {}, {});
    }
}

void MainComponent::StockChanged(const std::string& stock_id)
{
    stock_id_ = stock_id;
    chart_scroll_bar_.setRangeLimits(0, std::get<0>(lei::GetKDataCenter().GetKData(stock_id_, data_frequency_)).size());
    chart_scroll_bar_.scrollToBottom();

    const auto current_range = ToInt(chart_scroll_bar_.getCurrentRange());
    k_index_ = current_range.getEnd() - 1;

    watch_tool_.Clear();

    for (const auto& indicator : main_indicators_)
    {
        indicator->StockChanged();
    }

    for (const auto& indicator : subsidiary_indicators_)
    {
        indicator->StockChanged();
    }

    HandleZoomChanged();
}

void MainComponent::DataFrequencyChanged(lei::DataFrequency frequency)
{
    data_frequency_ = frequency;
    chart_scroll_bar_.setRangeLimits(0, std::get<0>(lei::GetKDataCenter().GetKData(stock_id_, data_frequency_)).size());
    chart_scroll_bar_.scrollToBottom();
    k_index_ = 0;
    watch_tool_.Clear();

    for (const auto& indicator : main_indicators_)
    {
        indicator->StockChanged();
    }

    for (const auto& indicator : subsidiary_indicators_)
    {
        indicator->StockChanged();
    }

    HandleZoomChanged();
}

void MainComponent::ToolChanged(lei::ToolType tool_type)
{
    if (tool_type_ == tool_type)
    {
        return;
    }

    tool_type_ = tool_type;
    tool_ = lei::ToolFactory::GetTool(tool_type,
                                      this,
                                      std::bind(&MainComponent::GetKArray, this),
                                      GetTools(stock_id_, data_frequency_),
                                      std::bind(&MainComponent::RegisterEraseButton, this, std::placeholders::_1),
                                      std::bind(&MainComponent::UnregisterEraseButton, this, std::placeholders::_1));
}

int MainComponent::CalculateScreenKSize() const
{
    return (k_chart_bounds_.reduced(lei::kChartBorderThickness).getWidth() - lei::kBarGap) / (bar_width_ + lei::kBarGap);
}

int MainComponent::GetKIndexRestrictInBounds(const juce::Point<int>& pt) const
{
    const auto bounds = k_chart_bounds_.reduced(lei::kChartBorderThickness);
    const auto current_range = ToInt(chart_scroll_bar_.getCurrentRange());
    int index = current_range.getStart() + (pt.getX() - bounds.getX() - lei::kBarGap / 2.0) / (bar_width_ + lei::kBarGap);
    index = std::max(index, current_range.getStart());
    index = std::min(index, current_range.getEnd() - 1);
    return index;
}

int MainComponent::GetKCentreXRestrictInBounds(const juce::Point<int>& pt) const
{
    const auto bounds = k_chart_bounds_.reduced(lei::kChartBorderThickness);
    const auto distance = GetKIndexRestrictInBounds(pt) - juce::roundToInt(chart_scroll_bar_.getCurrentRangeStart());
    return bounds.getX() + lei::kBarGap + distance * (bar_width_ + lei::kBarGap) + bar_width_ / 2.0; // don't use round to int, because bar_bounds.getCentreX in K::DrawKBar not use.
}

int MainComponent::GetChartIndex(const juce::Point<int>& pt) const
{
    if (pt.getY() <= k_chart_bounds_.getBottom())
    {
        return 0;
    }
    else if (pt.getY() >= subsidiary_charts_bounds_.rbegin()->getY())
    {
        return kSubsidiaryChartSize;
    }
    else
    {
        for (int i = 0; i < kSubsidiaryChartSize - 1; ++i)
        {
            if (subsidiary_charts_bounds_[i].getY() <= pt.getY() && pt.getY() <= subsidiary_charts_bounds_[i].getBottom())
            {
                return i + 1;
            }
        }
    }

    return -1;
}

juce::Rectangle<int> MainComponent::GetChartBounds(const juce::Point<int>& pt) const
{
    if (pt.getY() <= k_chart_bounds_.getBottom())
    {
        return k_chart_bounds_;
    }
    else if (pt.getY() >= subsidiary_charts_bounds_.rbegin()->getY())
    {
        return *subsidiary_charts_bounds_.rbegin();
    }
    else
    {
        for (int i = 0; i < kSubsidiaryChartSize - 1; ++i)
        {
            if (subsidiary_charts_bounds_[i].getY() <= pt.getY() && pt.getY() <= subsidiary_charts_bounds_[i].getBottom())
            {
                return subsidiary_charts_bounds_[i];
            }
        }
    }

    return {};
}

juce::Rectangle<int> MainComponent::GetChartBounds(int chart_index) const
{
    if (chart_index == 0)
    {
        return k_chart_bounds_;
    }
    else if (1 <= chart_index && chart_index <= kSubsidiaryChartSize)
    {
        return subsidiary_charts_bounds_[chart_index - 1];
    }

    return {};
}

juce::Rectangle<int> MainComponent::GetPriceLabelBounds(const juce::Point<int>& pt) const
{
    if (pt.getY() <= k_chart_bounds_.getBottom())
    {
        return k_price_label_bounds_;
    }
    else if (pt.getY() >= subsidiary_charts_bounds_.rbegin()->getY())
    {
        return *subsidiary_labels_bounds_.rbegin();
    }
    else
    {
        for (int i = 0; i < kSubsidiaryChartSize - 1; ++i)
        {
            if (subsidiary_charts_bounds_[i].getY() <= pt.getY() && pt.getY() <= subsidiary_charts_bounds_[i].getBottom())
            {
                return subsidiary_labels_bounds_[i];
            }
        }
    }

    return {};
}

std::pair<double, double> MainComponent::GetChartMinMaxLabel(const juce::Point<int>& pt) const
{
    if (pt.getY() <= k_chart_bounds_.getBottom())
    {
        return k_chart_min_max_label_;
    }
    else if (pt.getY() >= subsidiary_charts_bounds_.rbegin()->getY())
    {
        return (*subsidiary_indicators_.rbegin())->GetMinMaxLabelValue();
    }
    else
    {
        for (int i = 0; i < kSubsidiaryChartSize - 1; ++i)
        {
            if (subsidiary_charts_bounds_[i].getY() <= pt.getY() && pt.getY() <= subsidiary_charts_bounds_[i].getBottom())
            {
                return subsidiary_indicators_[i]->GetMinMaxLabelValue();
            }
        }
    }

    return {};
}

std::pair<double, double> MainComponent::GetChartMinMaxLabel(int chart_index) const
{
    if (chart_index == 0)
    {
        return k_chart_min_max_label_;
    }
    else if (1 <= chart_index && chart_index <= kSubsidiaryChartSize)
    {
        return subsidiary_indicators_[chart_index - 1]->GetMinMaxLabelValue();
    }

    return {};
}

void MainComponent::RegisterEraseButton(const std::shared_ptr<juce::Button>& erase_button)
{
    erase_button->addListener(this);
}

void MainComponent::UnregisterEraseButton(const std::shared_ptr<juce::Button>& erase_button)
{
    erase_button->removeListener(this);
}

std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>> MainComponent::GetTools(const std::string& stock_id, lei::DataFrequency data_frequency) const
{
    const auto pos = tools_.find({ stock_id_, data_frequency_ });
    if (pos != tools_.end())
    {
        std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>> tools;
        for (const auto& tool : pos->second)
        {
            tools.emplace(tool);
        }

        return tools;
    }

    return {};
}
