#include "playerInfo.h"
#include "confirmActionControlContainer.h"

GuiConfirmActionControlContainer::GuiConfirmActionControlContainer(GuiContainer* owner, string id)
: GuiBox(owner, id)
{
}

GuiConfirmActionControlContainer::~GuiConfirmActionControlContainer()
{
    control_list.clear();
}

bool GuiConfirmActionControlContainer::addItem(string text, P<SpaceShip> obj, func_t confirm, func_t cancel, float timeout)
{
    if (std::find(item_list.begin(), item_list.end(), obj) != item_list.end())
        return false;
    
    GuiConfirmActionControl* item = new GuiConfirmActionControl(this, text, confirm, cancel, timeout);
    return addItem(item);
}

bool GuiConfirmActionControlContainer::addItem(GuiConfirmActionControl* item)
{
    if (control_list.size() < max_size)
    {
        show();
        control_list.push_back(item);
        return true;
    }
    else
    {
        item->confirm_callback();
        return false;
    }
}

void GuiConfirmActionControlContainer::update()
{
    if (control_list.size() == 0)
        return;
    
    float y_pos = 0;
    for (std::vector<GuiConfirmActionControl*>::iterator control = control_list.begin(); control != control_list.end();)
    {
        if ((*control)->actionPerformed() == false)
        {
            (*control)->setPosition(0, y_pos);
            y_pos += (*control)->getSize().y;
            control++;
        }
        else
        {
            (*control)->hide();
            control = control_list.erase(control);
        }
    }
    
    if (y_pos == 0)
    {
        LOG(DEBUG) << "HIDE!";
        hide();
        setVisible(false);
    }
    else
    {
        float x_size = getSize().x;
        setSize(x_size, y_pos);
    }
}
