#include "settingsgui.h"

#include "graphics/fontHandler.h"
#include <variant>
#include <algorithm>
#include <nds.h>

#define CURSOR_MIN 30
#define CURSOR_MAX (SCREEN_HEIGHT - 40)
#define CURSOR_HEIGHT (CURSOR_MAX - CURSOR_MIN)

void SettingsGUI::draw()
{
    if (_selectedPage < 0 || _pages.size() < 1 || _selectedPage >= _pages.size())
        return;
    if (inSub())
    {
        drawSub();
        return;
    }

    clearText();
    exitSub();
    drawTopText();
    printLarge(false, 6, 1, _pages[_selectedPage].title().c_str());

    for (int i = _topCursor; i < _bottomCursor; i++)
    {
        int selected = _pages[_selectedPage].options()[i].selected();

        if (i == _selectedOption)
        {
            printSmall(false, 4, 29 + (i - _topCursor) * 14, ">");
        }

        printSmall(false, 12, 30 + (i - _topCursor) * 14, _pages[_selectedPage].options()[i].displayName().c_str());
        printSmall(false, 194, 30 + (i - _topCursor) * 14, _pages[_selectedPage].options()[i].labels()[selected].c_str());
    }

    // Divide CURSOR_HEIGHT into _subOption->values().size() pieces and get the ith piece.
    // Integer division is good enough for this case.
    int scrollSections = CURSOR_HEIGHT / (_pages[_selectedPage].options().size() - 1);
    // Print a nice thick scroller.
    printSmall(false, 252, (scrollSections * (_selectedOption)) + CURSOR_MIN, "|");
    printSmall(false, 254, (scrollSections * (_selectedOption)) + CURSOR_MIN, "|");

    printSmallCentered(false, 173, "DSiMenu++");
}

void SettingsGUI::drawSub()
{
    clearText();
    drawTopText();
    int selected = _subOption->selected();

    for (int i = _subTopCursor; i < _subBottomCursor; i++)
    {
        if (i == selected)
        {

            printSmall(false, 4, 29 + (i - _subTopCursor) * 14, ">");
        }

        printSmall(false, 12, 30 + (i - _subTopCursor) * 14, _subOption->labels()[i].c_str());
    }

    // Divide CURSOR_HEIGHT into _subOption->values().size() pieces and get the ith piece.
    // Integer division is good enough for this case.
    int scrollSections = CURSOR_HEIGHT / (_subOption->values().size() - 1);
    // Print a nice thick scroller.
    printSmall(false, 252, (scrollSections * (selected)) + CURSOR_MIN, "|");
    printSmall(false, 254, (scrollSections * (selected)) + CURSOR_MIN, "|");

    printLarge(false, 6, 1, _subOption->displayName().c_str());
    printSmallCentered(false, 173, "DSiMenu++");
}

void SettingsGUI::drawTopText()
{
    
    for (int i = 0; i < _topText.size(); i++) 
    {
        printLargeCentered(true, 96 + (i * 16), _topText[i].c_str());
    }
    // 
}

void SettingsGUI::rotateOptionValue(int rotateAmount)
{
    // Only the main menu pages have left-right option values. Sub menus only control one Option.
    if (inSub())
        return;

    auto selectedOption = _pages[_selectedPage].options()[_selectedOption];
    int currentValueIndex = selectedOption.selected();
    int nextValueIndex = (currentValueIndex + rotateAmount) % (selectedOption.values().size());
    if (currentValueIndex == -1)
        nextValueIndex = 0;
    auto nextValue = selectedOption.values()[nextValueIndex];

    if (auto action = std::get_if<Option::Bool>(&selectedOption.action()))
    {
        action->set(*std::get_if<bool>(&nextValue));
    }

    if (auto action = std::get_if<Option::Int>(&selectedOption.action()))
    {
        action->set(*std::get_if<int>(&nextValue));
    }

    if (auto action = std::get_if<Option::Str>(&selectedOption.action()))
    {
        nocashMessage(*std::get_if<cstr>(&nextValue));
        action->set(*std::get_if<cstr>(&nextValue));
    }
    clearText();
}

void SettingsGUI::rotateOption(int rotateAmount)
{
    if (!inSub())
    {
        // If we're not in the sub option menu, change the option.
        if ((_selectedOption + rotateAmount) < 0 || ((_selectedOption + rotateAmount) >= _pages[_selectedPage].options().size()))
        {
            return;
        }

        if (_selectedOption + rotateAmount >= _bottomCursor)
        {
            _topCursor++;
            _bottomCursor++;
        }

        if (_selectedOption + rotateAmount < _topCursor)
        {
            _topCursor--;
            _bottomCursor--;
        }

        setTopText(_pages[_selectedPage].options()[_selectedOption].longDescription());
        _selectedOption = (_selectedOption + rotateAmount) % (_pages[_selectedPage].options().size());
    }
    else
    {
        // Change the sub option instead.
        auto selectedOption = _pages[_selectedPage].options()[_selectedOption];
        auto &action = selectedOption.action_sub();
        if (action.sub())
        {
            auto sub = *action.sub();
            int currentValueIndex = sub.selected();

            // Update cursors.
            if ((currentValueIndex + rotateAmount) < 0 || ((currentValueIndex + rotateAmount) >= sub.values().size()))
            {
                // Prevent overflows...
                return;
            }

            if (currentValueIndex + rotateAmount >= _subBottomCursor)
            {
                _subTopCursor++;
                _subBottomCursor++;
            }

            if (currentValueIndex + rotateAmount < _subTopCursor)
            {
                _subTopCursor--;
                _subBottomCursor--;
            }

            int nextValueIndex = (currentValueIndex + rotateAmount) % (sub.values().size());
            if (currentValueIndex == -1)
                nextValueIndex = 0;
            auto nextValue = sub.values()[nextValueIndex];

            if (auto subaction = std::get_if<Option::Bool>(&sub.action()))
            {
                subaction->set(*std::get_if<bool>(&nextValue));
            }

            if (auto subaction = std::get_if<Option::Str>(&sub.action()))
            {
                subaction->set(*std::get_if<cstr>(&nextValue));
            }

            if (auto subaction = std::get_if<Option::Int>(&sub.action()))
            {
                int value = *std::get_if<int>(&nextValue);
                subaction->set(value);
            }
        }
    }

    clearText();
}