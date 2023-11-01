// © 2023 Lei Cheng

#include "MainMenu.h"

MainMenu::MainMenu()
{}

MainMenu::~MainMenu()
{}

juce::StringArray MainMenu::getMenuBarNames()
{
    return juce::StringArray(juce::translate("main menu"));
}

juce::PopupMenu MainMenu::getMenuForIndex(int top_level_menu_index, const juce::String& menu_name)
{
    juce::PopupMenu menu;
    menu.addItem(juce::translate("item 1"), [] {});
    menu.addItem(juce::translate("item 2"), [] {});
    return menu;
}

void MainMenu::menuItemSelected(int menu_item_id, int top_level_menu_index)
{

}
