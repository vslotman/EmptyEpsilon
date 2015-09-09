#include "hardwareMappingEffects.h"
#include "logging.h"
#include "hardwareController.h"
#include "gameGlobalInfo.h"
#include "playerInfo.h"
#include "spaceObjects/nebula.h"
#include "spaceObjects/warpJammer.h"

#define REQ_SETTING(key, variable, effect_name) \
    if (settings.find(key) == settings.end()) { LOG(ERROR) << "[" << key << "] not set for " << effect_name << " effect"; return false; } \
    variable = settings[key].toFloat();
#define OPT_SETTING(key, variable, effect_name, default) \
    if (settings.find(key) == settings.end()) { variable = default; } else { variable = settings[key].toFloat(); }

bool HardwareMappingEffectStatic::configure(std::unordered_map<string, string> settings)
{
    REQ_SETTING("value", value, "static");
    return true;
}

float HardwareMappingEffectStatic::onActive()
{
    return value;
}

bool HardwareMappingEffectGlow::configure(std::unordered_map<string, string> settings)
{
    OPT_SETTING("min_value", min_value, "glow", 0.0);
    OPT_SETTING("max_value", max_value, "glow", 1.0);
    REQ_SETTING("time", time, "glow");
    clock.restart();
    return true;
}

float HardwareMappingEffectGlow::onActive()
{
    if (clock.getElapsedTime().asSeconds() > time * 2.0)
        clock.restart();
    float f = clock.getElapsedTime().asSeconds() / time;
    if (f > 1.0)
        return min_value * (f - 1.0) + max_value * (2.0 - f);
    else
        return min_value * (1.0 - f) + max_value * (f);
}

void HardwareMappingEffectGlow::onInactive()
{
    clock.restart();
}

bool HardwareMappingEffectBlink::configure(std::unordered_map<string, string> settings)
{
    OPT_SETTING("off_value", off_value, "blink", 0.0);
    OPT_SETTING("on_value", off_value, "blink", 1.0);
    REQ_SETTING("on_time", on_time, "blink");
    REQ_SETTING("off_time", off_time, "blink");
    clock.restart();
    return true;
}

float HardwareMappingEffectBlink::onActive()
{
    if (clock.getElapsedTime().asSeconds() > on_time + off_time)
        clock.restart();
    if (clock.getElapsedTime().asSeconds() > on_time)
        return off_value;
    else
        return on_value;
}

void HardwareMappingEffectBlink::onInactive()
{
    clock.restart();
}


bool HardwareMappingEffectGameVariable::configure(std::unordered_map<string, string> settings)
{
    if (settings.find("variable") == settings.end()) 
    { 
        LOG(ERROR) << "[variable] not set for GameVariable effect"; 
        return false; 
    }
    else
    {
        variable = settings["variable"];
    }
    OPT_SETTING("interval", interval, "GameVariable", 0.0);
}

float HardwareMappingEffectGameVariable::onActive()
{
    if (clock.getElapsedTime().asSeconds() > interval)
    {
        clock.restart();
        getVariableValue(variable, value);
        //LOG(DEBUG) << "Refreshed " << variable << " to: " << value;
        return value;
    }
        
    else
        return value;
}

#define SHIP_VARIABLE(name, formula) if (variable_name == name) { if (ship) { value = (formula); return true; } return false; }
bool HardwareMappingEffectGameVariable::getVariableValue(string variable_name, float& value)
{
    P<PlayerSpaceship> ship = my_spaceship;
    if (!ship && gameGlobalInfo)
        ship = gameGlobalInfo->getPlayerShip(0);
    
    if (variable_name == "Always")
    {
        value = 1.0;
        return true;
    }
    if (variable_name == "HasShip")
    {
        value = bool(ship) ? 1.0f : 0.0f;
        return true;
    }
    SHIP_VARIABLE("Hull", 100.0f * ship->hull_strength / ship->hull_max);
    SHIP_VARIABLE("FrontShield", 100.0f * ship->front_shield / ship->front_shield_max);
    SHIP_VARIABLE("RearShield", 100.0f * ship->rear_shield / ship->rear_shield_max);
    SHIP_VARIABLE("Energy", ship->energy_level / 100.0);
    SHIP_VARIABLE("ShieldsUp", ship->shields_active ? 1.0f : 0.0f);
    SHIP_VARIABLE("Impulse", ship->current_impulse);
    SHIP_VARIABLE("Warp", ship->current_warp);
    SHIP_VARIABLE("Docking", ship->docking_state != DS_NotDocking ? 1.0f : 0.0f);
    SHIP_VARIABLE("Docked", ship->docking_state == DS_Docked ? 1.0f : 0.0f);
    SHIP_VARIABLE("InNebula", Nebula::inNebula(ship->getPosition()) ? 1.0f : 0.0f);
    SHIP_VARIABLE("IsJammed", WarpJammer::isWarpJammed(ship->getPosition()) ? 1.0f : 0.0f);
    SHIP_VARIABLE("Jumping", ship->jump_delay > 0.0f ? 1.0f : 0.0f);
    SHIP_VARIABLE("Jumped", ship->jump_indicator > 0.0f ? 1.0f : 0.0f);
    for(int n=0; n<max_weapon_tubes; n++)
    {
        SHIP_VARIABLE("TubeLoaded" + string(n), ship->weaponTube[n].state == WTS_Loaded ? 1.0f : 0.0f);
        SHIP_VARIABLE("TubeLoading" + string(n), ship->weaponTube[n].state == WTS_Loading ? 1.0f : 0.0f);
        SHIP_VARIABLE("TubeUnloading" + string(n), ship->weaponTube[n].state == WTS_Unloading ? 1.0f : 0.0f);
    }
    
    LOG(WARNING) << "Unknown variable: " << variable_name;
    value = 0.0;
    return false;
}

