// © 2023 Lei Cheng

#pragma once

#include <JuceHeader.h>

namespace lei
{
    enum class DataFrequency;

    juce::Font GeStockSearchBarFont();
    juce::Font GetHeaderFont();
    juce::Font GetValueLabelFont();
    juce::Font GetTimeLabelFont();
    juce::Font GetWatchToolMessageFont();

    juce::String GetTimeFormat(DataFrequency frequency);
}
