#ifndef HTTP_REQUEST_DEVICE_H
#define HTTP_REQUEST_DEVICE_H

#include <SFML/System.hpp>
//#include <stdint.h>
#include "hardwareOutputDevice.h"
#include "httpRequestHandler.h"
//#include "stringImproved.h"


//The HTTPRequestDevice can talk to Open DMX USB hardware, and just about any hardware which is just an serial port connected to a line driver.
class HTTPRequestDevice : public HardwareOutputDevice
{
private:
    int timeout;
    sf::Time refresh_interval;
    std::unordered_map<int, HTTPChannel*> channels;
    std::vector<HTTPRequestHandler*> active_requests;
    int max_active_requests;
    sf::Thread update_thread;
    
    bool run_thread;
public:
    HTTPRequestDevice();
    virtual ~HTTPRequestDevice();
    
    //Configure the device.
    // Parameter: port: name of the serial port to connect to.
    virtual bool configure(std::unordered_map<string, string> settings);

    virtual bool configureChannel(int channel_id, std::unordered_map<string, string> settings);
    
    //void addChannelHost(int channel, string host);
    
    //void addChannelUri(int channel, string uri);
    //void add

    //Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
    virtual void setChannelData(int channel, float value);
    //virtual void setChannelData(int channel, std::unordered_map<string, string> parameters);

    //Return the number of output channels supported by this device.
    virtual int getChannelCount();
private:
    void updateLoop();
};


#endif//HTTP_REQUEST_DEVICE_H
