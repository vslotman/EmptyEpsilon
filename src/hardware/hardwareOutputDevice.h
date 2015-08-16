#ifndef HARDWARE_OUTPUT_DEVICE_H
#define HARDWARE_OUTPUT_DEVICE_H

#include <unordered_map>
#include <array>
#include "stringImproved.h"

typedef std::array<bool, 512> channelMask;

class HardwareOutputDevice
{
public:
    HardwareOutputDevice();
    virtual ~HardwareOutputDevice();
    
    virtual bool configure(std::unordered_map<string, string> settings) = 0;
    
    virtual bool configureChannel(int channel_id, std::unordered_map<string, string> settings) = 0;

    //Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
    virtual void setChannelData(int channel, float value) = 0;
    
    //Return the number of output channels supported by this device.
    virtual int getChannelCount() = 0;
    
    virtual channelMask configureChannelMask(string channel_string);
};

#endif//HARDWARE_OUTPUT_DEVICE_H
