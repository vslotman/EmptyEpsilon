/*
 * sevcol.cpp
 *
 *  Created on: 10 nov. 2016
 *      Author: Vincent Slotman
 */
#include "sevcol.h"

#include "gui/gui2_listbox.h"
#include "gui/gui2_autolayout.h"
#include "gui/gui2_keyvaluedisplay.h"
#include "gui/gui2_label.h"
#include "gui/gui2_textentry.h"
#include "gui/gui2_selector.h"
#include "gui/gui2_slider.h"
#include "gui/gui2_togglebutton.h"


GuiShipTweakSevcol::GuiShipTweakSevcol(GuiContainer* owner)
: GuiTweakPage(owner)
{
	GuiAutoLayout* left_col = new GuiAutoLayout(this, "LEFT_LAYOUT", GuiAutoLayout::LayoutVerticalTopToBottom);
	left_col->setPosition(50, 25, ATopLeft)->setSize(300, GuiElement::GuiSizeMax);

	GuiAutoLayout* right_col = new GuiAutoLayout(this, "RIGHT_LAYOUT", GuiAutoLayout::LayoutVerticalTopToBottom);
	right_col->setPosition(-25, 25, ATopRight)->setSize(300, GuiElement::GuiSizeMax);

	// Left column
	// Toggle if object is allowed to be destroyed or not
	(new GuiLabel(left_col, "", "Allow destruction:", 30))->setSize(GuiElement::GuiSizeMax, 50);
	can_be_destroyed_toggle = new GuiToggleButton(left_col, "", "YES", [this](bool value) {
			target->can_be_destroyed = value;
		});
	can_be_destroyed_toggle->setSize(GuiElement::GuiSizeMax, 40);


}

void GuiShipTweakSevcol::onDraw(sf::RenderTarget& window)
{
	if (this->target->can_be_destroyed)
		can_be_destroyed_toggle->setText("YES");
	else
		can_be_destroyed_toggle->setText("NO");
}

void GuiShipTweakSevcol::open(P<SpaceObject> target)
{
	P<SpaceShip> ship = target;
	this->target = ship;

	can_be_destroyed_toggle->setValue(ship->can_be_destroyed);
}
