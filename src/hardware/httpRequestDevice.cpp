#include "httpRequestDevice.h"
#include "logging.h"

#define MAX_CONCURRENT_REQUESTS 32

HTTPRequestDevice::HTTPRequestDevice()
{
    //returnCode = NULL;
}

HTTPRequestDevice::~HTTPRequestDevice()
{
}

bool HTTPRequestDevice::configure(std::unordered_map<string, string> settings)
{
    if (settings.find("timeout") != settings.end())
        timeout = settings["timeout"].toInt();
    if (settings.find("channels") != settings.end())
    {
        channel_count = std::max(1, std::min(512, settings["channels"].toInt()));
    }
    return false;
}

void HTTPRequestDevice::addChannel(int channel_id, string host, string uri)
{
    HTTPChannel channel;
    channel.host = host;
    channel.uri = uri;
    channels[channel_id] = channel;
}

//Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
void HTTPRequestDevice::setChannelData(int channel, float value)
{
    if (channels.size() < MAX_CONCURRENT_REQUESTS)
    {
        HTTPRequestHandler request((HTTPChannel*) &(channels[channel]));
        
    }
    else
    {
        // Do some logging
    }
}

//Return the number of output channels supported by this device.
int HTTPRequestDevice::getChannelCount()
{
    return channels.size();
}
