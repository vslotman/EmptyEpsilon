#ifndef GUI2_TOGGLE_BUTTON_H
#define GUI2_TOGGLE_BUTTON_H

#include "gui2_button.h"

class GuiToggleButton : public GuiButton
{
public:
    typedef std::function<void(bool active)> func_t;
private:
    bool value;
    func_t toggle_func;
    sf::Color selected_color;
    sf::Color unselected_color;
    
public:
    GuiToggleButton(GuiContainer* owner, string id, string text, func_t func);
    
    bool getValue();
    GuiToggleButton* setValue(bool value);
private:
    void onClick();
};

class GuiToggleButtonWithoutMouse : public GuiToggleButton
{
public:
    GuiToggleButtonWithoutMouse(GuiContainer* owner, string id, string text, func_t func)
        : GuiToggleButton(owner, id, text, func) {}
    virtual bool onMouseDown(sf::Vector2f position) { return false; }
    virtual void onMouseDrag(sf::Vector2f position) {}
    virtual void onMouseUp(sf::Vector2f position)   {}
};


#endif//GUI2_TOGGLE_BUTTON_H
