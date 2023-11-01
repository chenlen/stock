// © 2023 Lei Cheng

#include "ToolFactory.h"
#include "Tool/EraseTool.h"
#include "Tool/HorizontalLineTool.h"
#include "Tool/LineTool.h"
#include "Tool/NoneTool.h"
#include "Tool/ParallelLinesTool.h"
#include "Tool/TimeFibonacciSequenceTool.h"
#include "Tool/TrendlineTool.h"
#include "Tool/VerticalLineTool.h"
#include "ToolType.h"

namespace lei
{
    ToolFactory::ToolFactory()
    {}

    ToolFactory::~ToolFactory()
    {}

    std::unique_ptr<Tool> ToolFactory::GetTool(ToolType type,
                                               juce::Component* component,
                                               const std::function<const KArray& ()>& GetKArray,
                                               const std::unordered_map<juce::Uuid, std::weak_ptr<lei::Tool>>& tools,
                                               const std::function<void(const std::shared_ptr<juce::Button>&)>& RegisterEraseButton,
                                               const std::function<void(const std::shared_ptr<juce::Button>&)>& UnregisterEraseButton)
    {
        switch (type)
        {
        case lei::ToolType::kNone:
            return std::make_unique<lei::NoneTool>(component);
        case lei::ToolType::kVerticalLine:
            return std::make_unique<lei::VerticalLineTool>(component, GetKArray);
        case lei::ToolType::kHorizontalLine:
            return std::make_unique<lei::HorizontalLineTool>(component, GetKArray);
        case lei::ToolType::kTrendline:
            return std::make_unique<lei::TrendlineTool>(component, GetKArray);
        case lei::ToolType::kParallelLines:
            return std::make_unique<lei::ParallelLinesTool>(component, GetKArray);
        case lei::ToolType::kLine:
            return std::make_unique<lei::LineTool>(component, GetKArray);
        case lei::ToolType::kTimeFibonacciSequence:
            return std::make_unique<lei::TimeFibonacciSequenceTool>(component, GetKArray);
        case lei::ToolType::kErase:
            return std::make_unique<lei::EraseTool>(component, tools, RegisterEraseButton, UnregisterEraseButton);
        default:
            break;
        }

        return {};
    }
}
