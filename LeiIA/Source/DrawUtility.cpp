// © 2023 Lei Cheng

#include "DrawUtility.h"
#include "DataFrequency.h"

namespace lei
{
    juce::Font GeStockSearchBarFont()
    {
        return { "Microsoft JhengHei", 18, juce::Font::bold };
    }

    juce::Font GetHeaderFont()
    {
        return { "Microsoft JhengHei", 24, juce::Font::bold };
    }

    juce::Font GetValueLabelFont()
    {
        return { "Microsoft JhengHei", 18, juce::Font::bold };
    }

    juce::Font GetTimeLabelFont()
    {
        return { "Microsoft JhengHei", 18, juce::Font::bold };
    }

    juce::Font GetWatchToolMessageFont()
    {
        return { "Microsoft JhengHei", 18, juce::Font::bold };
    }

    juce::String GetTimeFormat(DataFrequency frequency)
    {
        switch (frequency)
        {
        case lei::DataFrequency::k1Min:
            return "%Y/%m/%d %H:%M:%S";
        case lei::DataFrequency::kDay:
            return "%Y/%m/%d";
        default:
            break;
        }

        return "";
    }
}
