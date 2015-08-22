#ifndef HARDWARE_OUTPUT_DEVICE_H
#define HARDWARE_OUTPUT_DEVICE_H

#include <unordered_map>
#include <array>
#include "stringImproved.h"

#define DEFAULT_CHANNEL_COUNT   512

// Array o booleans. true means channel is active.
typedef std::array<bool, DEFAULT_CHANNEL_COUNT> channelMask;

class HardwareOutputDevice
{
public:
    HardwareOutputDevice();
    virtual ~HardwareOutputDevice();
    
    virtual bool configure(std::unordered_map<string, string> settings) = 0;
    
    //Setup a channel. Settings are from [channel]-section of hardware.ini
    virtual bool configureChannel(int channel_id, std::unordered_map<string, string> settings) = 0;

    //Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
    virtual void setChannelData(int channel, float value) = 0;
    
    //Return the number of output channels supported by this device.
    virtual int getChannelCount() = 0;
    
    //Return a channelMask. Can be used to mask channel activity.
    virtual channelMask configureChannelMask(string channel_string);

    // Offset of a channel. Allows multiple independent devices.
    int channel_offset = 0;
};

#endif//HARDWARE_OUTPUT_DEVICE_H
