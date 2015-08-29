#include "hardwareController.h"
#include "serialDriver.h"
#include "logging.h"
#include "gameGlobalInfo.h"
#include "playerInfo.h"
#include "spaceObjects/nebula.h"
#include "spaceObjects/warpJammer.h"

#include "dmx512SerialDevice.h"
#include "enttecDMXProDevice.h"
#include "virtualOutputDevice.h"
#include "hardwareMappingEffects.h"

HardwareController::HardwareController()
{
}

HardwareController::~HardwareController()
{
    for(HardwareOutputDevice* device : devices)
        delete device;
    for(HardwareMappingState& state : states)
        delete state.effect;
}

void HardwareController::loadConfiguration(string filename)
{
    FILE* f = fopen(filename.c_str(), "r");
    if (!f)
    {
        LOG(INFO) << filename << " not found. Not controlling external hardware.";
        return;
    }

    std::unordered_map<string, string> settings;
    string section = "";
    char buffer[512];
    while(fgets(buffer, sizeof(buffer), f))
    {
        string line = string(buffer).strip();
        if(line.find("#") > -1)
            line = line.substr(0, line.find("#")).strip();
        if (line.startswith("[") && line.endswith("]"))
        {
            if (section != "")
            {
                handleConfig(section, settings);
                settings.clear();
            }
            section = line;
        }else if (line.find("=") > -1)
        {
            string key = line.substr(0, line.find("=")).strip();
            string value = line.substr(line.find("=") + 1).strip();
            settings[key] = value;
        }
    }
    if (section != "")
        handleConfig(section, settings);
    
    fclose(f);

    channels.resize(0);
    for(HardwareOutputDevice* device : devices)
    {
        channels.resize(channels.size() + device->getChannelCount(), 0.0f);
    }
    LOG(INFO) << "Hardware subsystem initialized with: " << channels.size() << " channels";
    
    if (devices.size() < 1)
    {
        LOG(INFO) << "List of available serial ports:";
        for(string port : SerialPort::getAvailablePorts())
        {
            LOG(INFO) << port << " - " << SerialPort::getPseudoDriverName(port);
        }
    }
}

void HardwareController::handleConfig(string section, std::unordered_map<string, string> settings)
{
    if (section == "[hardware]")
    {
        HardwareOutputDevice* device   = nullptr;
        //HTTPRequestDevice http_device  = nullptr;
        
        if (settings["device"] == "")
            LOG(ERROR) << "No device definition in [hardware] section";
        else if (settings["device"] == "DMX512SerialDevice")
            device = new DMX512SerialDevice();
        else if (settings["device"] == "EnttecDMXProDevice")
            device = new EnttecDMXProDevice();
        else if (settings["device"] == "VirtualOutputDevice")
            device = new VirtualOutputDevice();
        else if ( (settings["device"] == "HttpRequestDevice") && (http_device == nullptr))
        {
            LOG(INFO) << "Added HTTP-device";
            //device = new HTTPRequestDevice();   
            http_device = new HTTPRequestDevice();
            device = (HardwareOutputDevice*) http_device;
            LOG(INFO) << "New hardware device: " << settings["device"];
            //devices.push_back(http_device);    
        }
        if (device)
        {
            if (!device->configure(settings))
            {
                LOG(ERROR) << "Failed to configure device: " << settings["device"];
                delete device;
            }else{
                LOG(INFO) << "New hardware device: " << settings["device"] << " with: " << device->getChannelCount() << " channels";
                devices.push_back(device);
            }
        }
        
        
        
    }
    else if(section == "[channel]")
    {
        if (settings["channel"] == "" || settings["name"] == "")
            LOG(ERROR) << "Incorrect properties in [channel] section";
        else
        {
            channel_mapping[settings["name"]] = settings["channel"].toInt();
            LOG(INFO) << "Channel #" << settings["channel"] << ": " << settings["name"];
            if (settings["host"] != "")
            {
                if (http_device != nullptr)
                    http_device->configureChannel(settings["channel"].toInt(), settings);
                else
                    LOG(ERROR) << "No HttpRequestDevice configured, but we have channel-config for it";
            }
        }
    }else if(section == "[channels]")
    {
        for(std::pair<string, string> item : settings)
        {
            channel_mapping[item.first] = item.second.toInt();
            LOG(INFO) << "Channel #" << item.second << ": " << item.first;
        }
    }else if(section == "[state]")
    {
        string condition = settings["condition"];
        
        HardwareMappingState state;
        state.variable = condition;
        state.compare_operator = HardwareMappingState::Greater;
        state.compare_value = 0.0;
        state.channel_nr = -1;
        if (channel_mapping.find(settings["target"]) != channel_mapping.end())
            state.channel_nr = channel_mapping[settings["target"]];
        
        for(HardwareMappingState::EOperator compare_operator : {HardwareMappingState::Less, HardwareMappingState::Greater, HardwareMappingState::Equal, HardwareMappingState::NotEqual})
        {
            string compare_string = "<";
            switch(compare_operator)
            {
            case HardwareMappingState::Less: compare_string = "<"; break;
            case HardwareMappingState::Greater: compare_string = ">"; break;
            case HardwareMappingState::Equal: compare_string = "=="; break;
            case HardwareMappingState::NotEqual: compare_string = "!="; break;
            }
            if (condition.find(compare_string) > -1)
            {
                state.variable = condition.substr(0, condition.find(compare_string)).strip();
                state.compare_operator = compare_operator;
                state.compare_value = condition.substr(condition.find(compare_string) + 1).strip().toFloat();
            }
        }
        
        if (state.channel_nr < 0)
        {
            LOG(ERROR) << "Unknown target channel in hardware.ini: " << settings["target"];
        }else{
            state.effect = createEffect(settings);
            
            if (state.effect)
            {
                LOG(DEBUG) << "New hardware state: " << state.channel_nr << ":" << state.variable << " " << state.compare_operator << " " << state.compare_value;
                states.push_back(state);
            }
        }
    }else if(section == "[event]")
    {
        string trigger = settings["trigger"];
        
        HardwareMappingEvent event;
        event.compare_operator = HardwareMappingEvent::Change;
        if (trigger.startswith("<"))
        {
            event.compare_operator = HardwareMappingEvent::Decrease;
            trigger = trigger.substr(1).strip();
        }
        if (trigger.startswith(">"))
        {
            event.compare_operator = HardwareMappingEvent::Increase;
            trigger = trigger.substr(1).strip();
        }
        event.trigger_variable = trigger;
        event.channel_nr = -1;
        event.runtime = settings["runtime"].toFloat();
        event.triggered = false;
        event.previous_value = 0.0;
        
        if (channel_mapping.find(settings["target"]) != channel_mapping.end())
            event.channel_nr = channel_mapping[settings["target"]];
        
        if (event.channel_nr < 0)
        {
            LOG(ERROR) << "Unknown target channel in hardware.ini: " << settings["target"];
        }else{
            event.effect = createEffect(settings);
            if (event.effect)
            {
                LOG(DEBUG) << "New hardware event: " << event.channel_nr << ":" << event.trigger_variable << " " << event.compare_operator;
                events.push_back(event);
            }
        }
    }else{
        LOG(ERROR) << "Unknown section in hardware.ini: " << section;
    }
}

void HardwareController::update(float delta)
{
    if (channels.size() < 1)
        return;
    for(float& value : channels)
        value = 0.0;
    for(HardwareMappingState& state : states)
    {
        float value;
        bool active = false;
        if (getVariableValue(state.variable, value))
        {
            switch(state.compare_operator)
            {
            case HardwareMappingState::Less: active = value < state.compare_value; break;
            case HardwareMappingState::Greater: active = value > state.compare_value; break;
            case HardwareMappingState::Equal: active = value == state.compare_value; break;
            case HardwareMappingState::NotEqual: active = value != state.compare_value; break;
            }
        }
        
        if (active && state.channel_nr < int(channels.size()))
        {
            channels[state.channel_nr] = state.effect->onActive();
        }else{
            state.effect->onInactive();
        }
    }
    for(HardwareMappingEvent& event : events)
    {
        float value;
        bool trigger = false;
        if (getVariableValue(event.trigger_variable, value))
        {
            if (event.previous_valid)
            {
                switch(event.compare_operator)
                {
                case HardwareMappingEvent::Change:
                    if (fabs(event.previous_value - value) > 0.1)
                        trigger = true;
                    break;
                case HardwareMappingEvent::Increase:
                    if (value > event.previous_value + 0.1)
                        trigger = true;
                    break;
                case HardwareMappingEvent::Decrease:
                    if (value < event.previous_value - 0.1)
                        trigger = true;
                    break;
                }
            }
            event.previous_value = value;
            event.previous_valid = true;
        }else{
            event.previous_valid = false;
        }
        if (trigger)
        {
            event.triggered = true;
            event.start_time.restart();
        }
        if (event.triggered && event.channel_nr < int(channels.size()))
        {
            channels[event.channel_nr] = event.effect->onActive();
            if (event.start_time.getElapsedTime().asSeconds() > event.runtime)
                event.triggered = false;
            event.effect->onInactive();
        }else{
        }
    }

    int idx = 0;
    for(HardwareOutputDevice* device : devices)
    {
        for(int n=0; n<device->getChannelCount(); n++)
            device->setChannelData(n, channels[idx++]);
    }
}

HardwareMappingEffect* HardwareController::createEffect(std::unordered_map<string, string>& settings)
{
    HardwareMappingEffect* effect = nullptr;
    string effect_name = settings["effect"].lower();
    if (effect_name == "static" || effect_name == "")
        effect = new HardwareMappingEffectStatic();
    else if (effect_name == "glow")
        effect = new HardwareMappingEffectGlow();
    else if (effect_name == "blink")
        effect = new HardwareMappingEffectBlink();
    
    if (effect->configure(settings))
        return effect;
    delete effect;
    return nullptr;
}

#define SHIP_VARIABLE(name, formula) if (variable_name == name) { if (ship) { value = (formula); return true; } return false; }
bool HardwareController::getVariableValue(string variable_name, float& value)
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
    SHIP_VARIABLE("Energy", ship->energy_level);
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
