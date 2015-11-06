#include "playerInfo.h"
#include "gameGlobalInfo.h"
#include "scienceScreen.h"
#include "spaceObjects/nebula.h"

#include "screenComponents/radarView.h"
#include "screenComponents/scanTargetButton.h"
#include "screenComponents/frequencyCurve.h"
#include "screenComponents/scanningDialog.h"
#include "screenComponents/databaseView.h"

ScienceScreen::ScienceScreen(GuiContainer* owner)
: GuiOverlay(owner, "SCIENCE_SCREEN", sf::Color::Black)
{
    radar_view = new GuiElement(this, "RADAR_VIEW");
    radar_view->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

    radar = new GuiRadarView(radar_view, "SCIENCE_RADAR", gameGlobalInfo->long_range_radar_range, &targets);
    radar->setPosition(20, 0, ACenterLeft)->setSize(GuiElement::GuiSizeMatchHeight, GuiElement::GuiSizeMax);
    radar->setRangeIndicatorStepSize(5000.0)->longRange()->enableWaypoints()->enableCallsigns()->enableHeadingIndicators()->setStyle(GuiRadarView::Circular)->setFogOfWarStyle(GuiRadarView::NebulaFogOfWar);
    radar->setCallbacks(
        [this](sf::Vector2f position) {
            if (!my_spaceship || my_spaceship->scanning_delay > 0.0)
                return;
            
            targets.setToClosestTo(position, 1000);
        }, nullptr, nullptr
    );
    
    GuiAutoLayout* sidebar = new GuiAutoLayout(radar_view, "SIDE_BAR", GuiAutoLayout::LayoutVerticalTopToBottom);
    sidebar->setPosition(-20, 50, ATopRight)->setSize(250, GuiElement::GuiSizeMax);
    (new GuiScanTargetButton(sidebar, "SCAN_BUTTON", &targets))->setSize(GuiElement::GuiSizeMax, 50);
    
    info_callsign = new GuiKeyValueDisplay(sidebar, "SCIENCE_CALLSIGN", 0.4, "Callsign", "");
    info_callsign->setSize(GuiElement::GuiSizeMax, 30);
    info_distance = new GuiKeyValueDisplay(sidebar, "SCIENCE_DISTANCE", 0.4, "Distance", "");
    info_distance->setSize(GuiElement::GuiSizeMax, 30);
    info_heading = new GuiKeyValueDisplay(sidebar, "SCIENCE_DISTANCE", 0.4, "Heading", "");
    info_heading->setSize(GuiElement::GuiSizeMax, 30);
    info_relspeed = new GuiKeyValueDisplay(sidebar, "SCIENCE_REL_SPEED", 0.4, "Rel.Speed", "");
    info_relspeed->setSize(GuiElement::GuiSizeMax, 30);
    
    info_faction = new GuiKeyValueDisplay(sidebar, "SCIENCE_FACTION", 0.4, "Faction", "");
    info_faction->setSize(GuiElement::GuiSizeMax, 30);
    info_type = new GuiKeyValueDisplay(sidebar, "SCIENCE_TYPE", 0.4, "Type", "");
    info_type->setSize(GuiElement::GuiSizeMax, 30);
    info_shields = new GuiKeyValueDisplay(sidebar, "SCIENCE_SHIELDS", 0.4, "Shields", "");
    info_shields->setSize(GuiElement::GuiSizeMax, 30);

    info_shield_frequency = new GuiFrequencyCurve(sidebar, "SCIENCE_SHIELD_FREQUENCY", false, true);
    info_shield_frequency->setSize(GuiElement::GuiSizeMax, 100);
    info_beam_frequency = new GuiFrequencyCurve(sidebar, "SCIENCE_SHIELD_FREQUENCY", true, false);
    info_beam_frequency->setSize(GuiElement::GuiSizeMax, 100);
    
    if (!gameGlobalInfo->use_beam_shield_frequencies)
    {
        info_shield_frequency->hide();
        info_beam_frequency->hide();
    }
    
    for(int n=0; n<SYS_COUNT; n++)
    {
        info_system[n] = new GuiKeyValueDisplay(sidebar, "SCIENCE_SYSTEM_" + string(n), 0.75, getSystemName(ESystem(n)), "-");
        info_system[n]->setSize(GuiElement::GuiSizeMax, 30);
        info_system[n]->hide();
    }
    
    info_description = new GuiScrollText(sidebar, "SCIENCE_DESC", "");
    info_description->setTextSize(20)->hide()->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    
    database_view = new DatabaseViewComponent(this);
    database_view->hide()->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);
    
    (new GuiListbox(this, "VIEW_SELECTION", [this](int index, string value) {
        radar_view->setVisible(index == 0);
        database_view->setVisible(index == 1);
    }))->setOptions({"Radar", "Database"})->setSelectionIndex(0)->setPosition(20, -20, ABottomLeft)->setSize(200, 100);
    
    new GuiScanningDialog(this, "SCANNING_DIALOG");
}

void ScienceScreen::onDraw(sf::RenderTarget& window)
{
    GuiOverlay::onDraw(window);
    if (!my_spaceship)
        return;
    if (targets.get() && Nebula::blockedByNebula(my_spaceship->getPosition(), targets.get()->getPosition()))
        targets.clear();

    info_faction->setValue("-");
    info_type->setValue("-");
    info_shields->setValue("-");
    info_shield_frequency->setFrequency(-1)->hide();
    info_beam_frequency->setFrequency(-1)->hide();

    for(int n=0; n<SYS_COUNT; n++)
        info_system[n]->setValue("-")->hide();
    
    if (targets.get())
    {
        P<SpaceObject> obj = targets.get();
        P<SpaceShip> ship = obj;
        P<SpaceStation> station = obj;
        sf::Vector2f position_diff = obj->getPosition() - my_spaceship->getPosition();
        float distance = sf::length(position_diff);
        float heading = sf::vector2ToAngle(position_diff) - 270;
        while(heading < 0) heading += 360;
        float rel_velocity = dot(obj->getVelocity(), position_diff / distance) - dot(my_spaceship->getVelocity(), position_diff / distance);
        if (fabs(rel_velocity) < 0.01)
            rel_velocity = 0.0;
        
        info_callsign->setValue(obj->getCallSign());
        info_distance->setValue(string(distance / 1000.0f, 1) + "km");
        info_heading->setValue(string(int(heading)));
        info_relspeed->setValue(string(rel_velocity / 1000.0f * 60.0f, 1) + "km/min");

        if (ship)
        {
            if (ship->scanned_by_player >= SS_SimpleScan)
            {
                info_faction->setValue(factionInfo[obj->getFactionId()]->getName());
                info_type->setValue(ship->ship_type_name);
                info_shields->setValue(string(int(ship->front_shield)) + ":" + string(int(ship->rear_shield)));
            }
            if (ship->scanned_by_player >= SS_FullScan)
            {
                if (gameGlobalInfo->use_beam_shield_frequencies)
                {
                    info_shield_frequency->setFrequency(ship->shield_frequency)->show();
                    info_beam_frequency->setFrequency(ship->beam_frequency)->show();
                }
                for(int n=0; n<SYS_COUNT; n++)
                {
                    info_system[n]->setValue(string(int(ship->systems[n].health * 100.0f)) + "%");
                    info_system[n]->show();
                }
            }
        }else{
            info_faction->setValue(factionInfo[obj->getFactionId()]->getName());
            if (station)
            {
                info_type->setValue(station->template_name);
                info_shields->setValue(string(int(station->shields)));
            }
        }
        string description = obj->getDescription();
        if (description.size() > 0)
        {
            info_description->setText(description)->show();
        }else{
            info_description->hide();
        }
    }else{
        info_callsign->setValue("-");
        info_distance->setValue("-");
        info_heading->setValue("-");
        info_relspeed->setValue("-");
    }
}
