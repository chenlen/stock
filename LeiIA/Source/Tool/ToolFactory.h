// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>
#include "Data/DataCenter.h"
#include "Tool/Tool.h"
#include <memory>

namespace lei
{
    enum class ToolType;

    class ToolFactory final
    {
    public:
        ToolFactory();
        ~ToolFactory();

    public:
        static std::unique_ptr<Tool> GetTool(ToolType type,
                                             juce::Component* component,
                                             const std::function<const KArray& ()>& GetKArray,
                                             const std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>>& tools,
                                             const std::function<void(const std::shared_ptr<juce::Button>&)>& RegisterEraseButton,
                                             const std::function<void(const std::shared_ptr<juce::Button>&)>& UnregisterEraseButton);
    };
}
