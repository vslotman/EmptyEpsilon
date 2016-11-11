#ifndef HTTP_REQUEST_DEVICE_H
#define HTTP_REQUEST_DEVICE_H

#include <SFML/System.hpp>
//#include <stdint.h>
#include <list>
#include "hardwareOutputDevice.h"
#include "httpRequestHandler.h"
#include "hardwareController.h"
//#include "stringImproved.h"


//The HTTPRequestDevice make HTTP-requests based on events. Requests are only sent on a change of value
class HTTPRequestDevice : public HardwareOutputDevice
{
private:
    sf::Clock start_time;
    bool enabled = false;
    float timeout;
    sf::Time refresh_interval;
    std::unordered_map<int, HTTPChannel*> channel_list;
    std::list<HTTPRequestHandler*> active_requests;
    std::vector<string> variables;
    int max_active_requests;
    sf::Thread update_thread;
    HardwareController* controller;
    
    bool run_thread;
public:
    HTTPRequestDevice(HardwareController* control);
    virtual ~HTTPRequestDevice();
    
    //Configure the device.
    // Parameter: port: name of the serial port to connect to.
    virtual bool configure(std::unordered_map<string, string> settings);

    // Configure the channel: Setup host, port, uri and other stuff.
    virtual bool configureChannel(int channel_id, std::unordered_map<string, string> settings);

    //Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
    virtual void setChannelData(int channel, float value);
    //virtual void setChannelData(int channel, std::unordered_map<string, string> parameters);

    //Return the number of output channels supported by this device.
    virtual int getChannelCount();
private:
    void updateLoop();
};


#endif//HTTP_REQUEST_DEVICE_H
