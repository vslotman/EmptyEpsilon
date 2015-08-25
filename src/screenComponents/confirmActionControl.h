#ifndef GUI_CONFIRM_ACTION_CONTROL_H
#define GUI_CONFIRM_ACTION_CONTROL_H

#include "gui/gui2.h"

class GuiConfirmActionControl : public GuiBox
{
public:
    typedef std::function<void()> func_t;
    func_t    confirm_callback;
    func_t    cancel_callback;
    
private:
    sf::Clock timer;
    sf::Time  confirm_timeout;
    bool      action_performed = false;
    
    GuiLabel*       box_label;
    GuiProgressbar* countdown_progressbar;
    GuiButton*      confirm_button;
    GuiButton*      cancel_button;    
    
public:
    GuiConfirmActionControl(GuiContainer* owner, string id, func_t confirm, func_t cancel, float timeout);
    ~GuiConfirmActionControl();
    
    bool actionPerformed() {return action_performed; };
    
    
    virtual void onDraw(sf::RenderTarget& window);
};

#endif//GUI_CONFIRM_ACTION_CONTROL_H
