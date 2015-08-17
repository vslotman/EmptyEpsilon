#include "hardwareOutputDevice.h"
#include "logging.h"

HardwareOutputDevice::HardwareOutputDevice()
{
}

HardwareOutputDevice::~HardwareOutputDevice()
{
}

channelMask HardwareOutputDevice::configureChannelMask(string channel_string)
{
    channelMask channel_mask_array;
    string      field;
    int         seperator;
    uint16_t    startPos;
    uint16_t    endPos;
    uint8_t     i;
    
    // Initialize the mask
    channel_mask_array.fill(false);
    
    if (channel_string.startswith("["))
    {
        LOG(DEBUG) << channel_string;
        channel_string = channel_string.substr(1, (channel_string.length() - 1));
        LOG(DEBUG) << channel_string;
        // We have an array
        for (string field : channel_string.split(","))
        {
            LOG(DEBUG) << field;
            seperator = field.find("-");
            LOG(DEBUG) << seperator;
            if (seperator == -1)
            {
                // No '-' found, it's just an integer
                channel_mask_array[field.toInt()] = true;
            }
            else
            {
                // We have a range
                startPos = field.substr(0, seperator).toInt();
                endPos   = field.substr(seperator+1, field.length()).toInt();
                if (endPos >= startPos)
                {
                    for (i = startPos; i < endPos; i++)
                    {
                        channel_mask_array[i] = true;
                    }
                }
            }
        }
    }
    //channel_count = std::max(1, std::min(512, settings["channels"].toInt()));
    return channel_mask_array;
}
