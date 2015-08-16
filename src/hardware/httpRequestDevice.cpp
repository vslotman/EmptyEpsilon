#include "httpRequestDevice.h"
#include "logging.h"

#define DEFAULT_MAX_REQUESTS     100
#define DEFAULT_REQUEST_INTERVAL 500

HTTPRequestDevice::HTTPRequestDevice()
: update_thread(&HTTPRequestDevice::updateLoop, this)
{
}

HTTPRequestDevice::~HTTPRequestDevice()
{
    if (run_thread)
    {
        run_thread = false;
        update_thread.wait();
    }
}

bool HTTPRequestDevice::configure(std::unordered_map<string, string> settings)
{
    if (settings["max_requests"] != "")
        max_active_requests = settings["max_requests"].toInt();
    else
        max_active_requests = DEFAULT_MAX_REQUESTS;
        
    if (settings["timeout"] != "")
        timeout = settings["timeout"].toInt();
    else
        timeout = 5;
        
    if (settings["refresh_interval"] != "")
        refresh_interval = sf::milliseconds(settings["refresh_interval"].toInt());
    else
        refresh_interval = sf::milliseconds(DEFAULT_REQUEST_INTERVAL);
        
    if (settings["channels"] != "")
        channel_mask = configureChannelMask(settings["channels"]);
        
    return true;
}

bool HTTPRequestDevice::configureChannel(int channel_id, std::unordered_map<string, string> settings)
{
    HTTPChannel* channel = new HTTPChannel();
    
    if (settings.find("host") == settings.end())
    {
        LOG(ERROR) << "No host configured for HTTP channel " << channel_id;
        return false;
    }
        
    else
        channel->host = settings["host"];
    
    if (settings.find("uri") != settings.end())
        channel->uri = settings["uri"];
    if (settings.find("port") != settings.end())
        channel->port = settings["port"].toInt();
    if (settings.find("timeout") != settings.end())
        channel->port = settings["timeout"].toInt();
        
    LOG(INFO) << channel->uri;

    channels[channel_id] = channel;
    LOG(INFO) << "added channel";
    
    return true;
}

//Set a hardware channel output. Value is 0.0 to 1.0 for no to max output.
void HTTPRequestDevice::setChannelData(int channel, float value)
{
    if (channels.find(channel) != channels.end() && 
        active_requests.size() <= DEFAULT_MAX_REQUESTS)
    {
        //HTTPRequestHandler request = nullptr;
        
        HTTPRequestHandler *request = new HTTPRequestHandler(channels[channel]);
        active_requests.push_back(request);
        LOG(DEBUG) << "Spawned request for channel #" << channel;
    }
    else
    {
        LOG(ERROR) << "Unknown HTTP channel id " << channel;
    }
}

//Return the number of output channels supported by this device.
int HTTPRequestDevice::getChannelCount()
{
    return 2;
}

void HTTPRequestDevice::updateLoop()
{
    uint32_t i;
    
    
    while (run_thread)
    {
        // Loop over vector of requests, and close each ready request-object
        for (i = 0; i < active_requests.size(); i++)
        {
            if (active_requests[i]->isReady()) //remove request-object if it has finished
                delete active_requests[i];
        }
        sf::sleep(refresh_interval);
    }
    
    // Wait for each request to have finished
    sf::Clock timer;
    while (active_requests.size() > 0)
    {
        sf::sleep(sf::milliseconds(100));
    
        // Loop over vector of requests, and close each ready request-object
        for (i = 0; i < active_requests.size(); i++)
        {
            if ( (active_requests[i]->isReady()) ||  //remove request-object if it has finished
                 (timer.getElapsedTime().asSeconds() < timeout)) //After timeout, thread gets killed by destructor
            {
                delete active_requests[i];
            }
        }        
    }
}
