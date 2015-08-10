#ifndef HTTP_REQUEST_DEVICE_H
#define HTTP_REQUEST_DEVICE_H

#include <SFML/System.hpp>
#include <stdint.h>
#include "hardwareOutputDevice.h"
#include "httpRequestHandler.h"

//The HTTPRequestDevice can talk to Open DMX USB hardware, and just about any hardware which is just an serial port connected to a line driver.
class HTTPRequestDevice : public HardwareOutputDevice
{
    uint8_t timeout;
    uint16_t channel_count;
    uint16_t active_requests;
    std::map<int, HTTPChannel> channels;
    std::vector<HTTPRequestHandler> requests;
public:
    HTTPRequestDevice();
    ~HTTPRequestDevice();
    //Configure the device.
    // Parameter: port: name of the serial port to connect to.
    virtual bool configure(std::unordered_map<string, string> settings);

    //virtual bool configureChannel(std::unordered_map<string, string> settings);
    virtual void addChannel(int channel_id, string host, string uri);

    //Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
    virtual void setChannelData(int channel, float value);
    //virtual void setChannelData(int channel, std::unordered_map<string, string> parameters);

    //Return the number of output channels supported by this device.
    virtual int getChannelCount();
};


#endif//HTTP_REQUEST_DEVICE_H
