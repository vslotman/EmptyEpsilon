#include "playerInfo.h"
#include "confirmActionControl.h"

GuiConfirmActionControl::GuiConfirmActionControl(GuiContainer* owner, string id, func_t confirm, func_t cancel, float timeout)
: GuiBox(owner, id)
{
    confirm_callback = confirm;
    cancel_callback  = cancel;
    confirm_timeout = sf::seconds(timeout);
    timer.restart();
    
    setSize(300, 150);
    
    box_label = new GuiLabel(this, id + "_CURRENT_LABEL", id, 30);
    box_label->setPosition(0, 10, ATopLeft)->setSize(GuiElement::GuiSizeMax, 30);
        
    
    confirm_button = new GuiButton(this, id + "_CONFIRM_BUTTON", "YES", [this]() {
        if (confirm_callback && !action_performed) 
        {
            confirm_callback();
            action_performed = true;
        }
    });
    confirm_button->setPosition(0, 100, ATopLeft)->setSize(150, 50);
    
    cancel_button = new GuiButton(this, id + "_CANCEL_BUTTON", "NO", [this]() {
        if (cancel_callback && !action_performed)
        {
            cancel_callback();
            action_performed = true;
        }
    });
   cancel_button->setPosition(150, 100, ATopLeft)->setSize(150, 50);
    
   countdown_progressbar = new GuiProgressbar(this, id + "_CONFIRM_COUNTDOWN", 0, timeout, timeout);
   countdown_progressbar->setPosition(0, 50, ATopLeft)->setSize(GuiElement::GuiSizeMax, 50);
}

void GuiConfirmActionControl::onDraw(sf::RenderTarget& window)
{
    if (!action_performed)
    {
        float remaining = (confirm_timeout - timer.getElapsedTime()).asSeconds();
    
        countdown_progressbar->setVisible(remaining > 0.0);
        countdown_progressbar->setValue(remaining);
        
        if (remaining <= 0.0)
        {
            confirm_callback();
            action_performed = true;
        }
    } 
    GuiBox::onDraw(window);
}

GuiConfirmActionControl::~GuiConfirmActionControl()
{
    LOG(DEBUG) << "destroy thingy";
    delete box_label;
    delete confirm_button;
    delete cancel_button;
    delete countdown_progressbar;
}
