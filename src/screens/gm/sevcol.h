/*
 * sevcol.h
 *
 *  Created on: 10 nov. 2016
 *      Author: Vincent
 */
#ifndef SRC_SCREENS_GM_SEVCOL_H_
#define SRC_SCREENS_GM_SEVCOL_H_

#include "tweak.h"


class GuiShipTweakSevcol : public GuiTweakPage
{
private:
    P<SpaceShip> target;

    GuiToggleButton* can_be_destroyed_toggle;
public:
    GuiShipTweakSevcol(GuiContainer* owner);

    virtual void open(P<SpaceObject> target);

    virtual void onDraw(sf::RenderTarget& window) override;
};

#endif /* SRC_SCREENS_GM_SEVCOL_H_ */
