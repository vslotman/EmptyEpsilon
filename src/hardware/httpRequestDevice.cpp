#include "httpRequestDevice.h"
#include "logging.h"

#define DEFAULT_MAX_REQUESTS     100
#define DEFAULT_REQUEST_INTERVAL 0.5
#define DEFAULT_TIMEOUT          5

HTTPRequestDevice::HTTPRequestDevice()
: update_thread(&HTTPRequestDevice::updateLoop, this)
{
    update_thread.launch();
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
        timeout = DEFAULT_TIMEOUT;
        
    if (settings["refresh_interval"] != "")
        refresh_interval = sf::seconds(settings["refresh_interval"].toInt());
    else
        refresh_interval = sf::seconds(DEFAULT_REQUEST_INTERVAL);
        
    return true;
}

// Parse the settings from hardware.ini to configure the channel
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
        channel->timeout = settings["timeout"].toInt();

    channel_list[channel_id] = channel;

    LOG(INFO) << "HTTPRequestDevice: Added channel #" <<channel_id << ": " << channel->host << ":" << channel->port << "/" << channel->uri;
    
    return true;
}

//Set a hardware channel output. For now, data only gets sent if value changes.
void HTTPRequestDevice::setChannelData(int channel, float value)
{      
    if (active_requests.size() >= DEFAULT_MAX_REQUESTS)
    {
        LOG(ERROR) << "Reached maximum concurrent requests";
        return;     
    }
    else if (!channel_list[channel])
    {
        return;
    }
    else if ( (channel_list.find(channel) != channel_list.end()) && 
              (channel_list[channel]->value != value))
    {
        channel_list[channel]->value = value;
        LOG(DEBUG) << "Spawning request for channel #" << channel << " with value: " << value;;
        
        HTTPRequestHandler *request = new HTTPRequestHandler(channel_list[channel]);
        active_requests.push_back(request);
    }
}

//Set a hardware channel output. For now, data only gets sent if value changes.
int HTTPRequestDevice::getChannelCount()
{
    /*
    int max_channel = 0;
    for (auto channel: channel_list)
    {
        if (channel.first > max_channel)
            max_channel = channel.first;
    }
    return (max_channel + 1);
    */
    return 50;
}

// Loop over list of active channels, removing those that have finished
void HTTPRequestDevice::updateLoop()
{
    uint32_t i;
    std::vector<HTTPRequestHandler*>::iterator it;
    
    while (run_thread)
    {
        // Loop over vector of requests, and close each ready request-object
        for (it = active_requests.begin(); it != active_requests.end(); it++)
        {
            if ((*it)->isReady()) //remove request-object if it has finished
            {
                delete * it;
                it = active_requests.erase(it);
            }
                
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
