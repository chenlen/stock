#pragma once

#include <JuceHeader.h>

class MainMenu : public juce::MenuBarModel
{
public:
    MainMenu();
    ~MainMenu() override;

public:
    juce::StringArray getMenuBarNames() override;
    juce::PopupMenu getMenuForIndex(int top_level_menu_index, const juce::String& menu_name) override;
    void menuItemSelected(int menu_item_id, int top_level_menu_index) override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainMenu)
};
