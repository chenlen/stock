// © 2023 Lei Cheng

#include "EraseTool.h"
#include "ToolType.h"

namespace lei
{
    EraseTool::EraseTool(juce::Component* target_component,
                         const std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>>& tools,
                         const std::function<void(const std::shared_ptr<juce::Button>&)>& RegisterEraseButton,
                         const std::function<void(const std::shared_ptr<juce::Button>&)>& UnregisterEraseButton) :
        component_(target_component),
        tools_(tools),
        RegisterEraseButton_(RegisterEraseButton),
        UnregisterEraseButton_(UnregisterEraseButton)
    {
        component_->setMouseCursor(juce::MouseCursor::NormalCursor);

        for (const auto& weak_tool : tools_)
        {
            if (auto tool = weak_tool.second.lock())
            {
                auto button = tool->GetEraseButton();
                button->setBounds(tool->GetEraseButtonBounds());
                RegisterEraseButton_(button);
                component_->addAndMakeVisible(button.get());
            }
        }
    }

    EraseTool::~EraseTool()
    {
        for (const auto& weak_tool : tools_)
        {
            if (auto tool = weak_tool.second.lock())
            {
                auto button = tool->GetEraseButton();
                component_->removeChildComponent(button.get());
                UnregisterEraseButton_(button);
            }
        }
    }

    void EraseTool::ToolBegin(const juce::Point<int>& position,
                              const juce::Rectangle<int>& chart_bounds,
                              const std::pair<double, double>& min_max_label,
                              const juce::Range<int>& scroll_bar_current_range,
                              int bar_width,
                              int chart_index)
    {
    }

    void EraseTool::ToolProcess(const juce::Point<int>& position)
    {
    }

    void EraseTool::ToolEnd(const juce::Point<int>& position)
    {
    }

    void EraseTool::ZoomChanged(const juce::Rectangle<int>&,
                                const std::pair<double, double>&,
                                const juce::Range<int>&,
                                int)
    {
        for (const auto& weak_tool : tools_)
        {
            if (auto tool = weak_tool.second.lock())
            {
                auto button = tool->GetEraseButton();
                button->setBounds(tool->GetEraseButtonBounds());
            }
        }
    }

    int EraseTool::GetChartIndex() const
    {
        return -1;
    }

    juce::Uuid EraseTool::GetKey() const
    {
        return {};
    }

    bool EraseTool::IsToolFinished() const
    {
        return false;
    }

    ToolType EraseTool::GetToolType() const
    {
        return ToolType::kErase;
    }

    juce::Rectangle<int> EraseTool::GetEraseButtonBounds() const
    {
        return {};
    }

    std::shared_ptr<juce::Button> EraseTool::GetEraseButton() const
    {
        return nullptr;
    }

    void EraseTool::Paint(juce::Graphics& g)
    {
    }
}
