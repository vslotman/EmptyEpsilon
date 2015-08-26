#ifndef GUI_SHIELD_FREQUENCY_SELECT_H
#define GUI_CONFIRM_ACTION_CONTROL_CONTAINER_H

#include "gui/gui2.h"
#include "screenComponents/confirmActionControl.h"

class GuiConfirmActionControlContainer : public GuiBox
{
public:
    typedef std::function<void()> func_t;
    
private:
    std::vector<GuiConfirmActionControl*> control_list;
    std::vector<P<SpaceShip>> item_list;
    
    int max_size = 5;
    
public:
    GuiConfirmActionControlContainer(GuiContainer* owner, string id);
    ~GuiConfirmActionControlContainer();
    
    void setMaxSize(int maximum_size) {max_size = maximum_size; };
    
    bool addItem(string text, P<SpaceShip> obj, func_t confirm, func_t cancel, float timeout);
    bool addItem(GuiConfirmActionControl*);
    void update();
};

#endif//GUI_CONFIRM_ACTION_CONTROL_CONTAINER_H
