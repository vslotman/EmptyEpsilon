#ifndef HTTP_REQUEST_HANDLER_H
#define HTTP_REQUEST_HANDLER_H

#include <SFML/System.hpp>
#include <SFML/Network/Http.hpp>
#include <SFML/System/Time.hpp>
#include <stdint.h>
#include "stringImproved.h"
#include <unordered_map>
#include "hardwareOutputDevice.h"

struct HTTPChannel
{
    string host;    // No default, must be set
    string uri      = "";
    uint16_t port   = 80;
    uint8_t timeout = 5;
    float value     = 0.1;
    bool  append_value = false;
};


//The HTTPRequestHandler can talk to Open DMX USB hardware, and just about any hardware which is just an serial port connected to a line driver.
class HTTPRequestHandler
{
    HTTPChannel        *target_channel;
    sf::Thread         request_thread;

    bool run_thread;
    bool is_ready = false;
public:
    // Initializer just an url and port
    HTTPRequestHandler(HTTPChannel * target);
    HTTPRequestHandler(HTTPChannel target, std::unordered_map<string, string> parameters);

    virtual ~HTTPRequestHandler();
    bool isReady();
private:
    void performRequest();
};


#endif//HTTP_REQUEST_HANDLER_H
