#include "httpRequestDevice.h"
#include "logging.h"
#include "stringImproved.h"


#define HTTP_DEFAULT_MAX_REQUESTS     200
#define HTTP_DEFAULT_REQUEST_INTERVAL 0.5


HTTPRequestDevice::HTTPRequestDevice(HardwareController* control)
: update_thread(&HTTPRequestDevice::updateLoop, this)
{
    LOG(INFO) << "Created HTTPRequestDevice";
    controller = control;
    start_time = sf::Clock();
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
        max_active_requests = HTTP_DEFAULT_MAX_REQUESTS;
        
    if (settings["timeout"] != "")
        timeout = settings["timeout"].toInt();
    else
        timeout = HTTP_DEFAULT_TIMEOUT;
        
    if (settings["refresh_interval"] != "")
        refresh_interval = sf::seconds(settings["refresh_interval"].toInt());
    else
        refresh_interval = sf::seconds(HTTP_DEFAULT_REQUEST_INTERVAL);
        
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
    else
        channel->port = HTTP_DEFAULT_PORT;

    if (settings.find("timeout") != settings.end())
        channel->timeout = settings["timeout"].toInt();

    if (settings.find("variables") != settings.end()) {
        settings["variables"].split(",");
    }

    channel_list[channel_id] = channel;

    LOG(INFO) << "HTTPRequestDevice: Added channel #" <<channel_id << ": " << channel->host << ":" << channel->port << "/" << channel->uri;
    
    return true;
}

//Set a hardware channel output. For now, data only gets sent if value changes.
void HTTPRequestDevice::setChannelData(int channel, float value)
{      
    if (active_requests.size() >= HTTP_DEFAULT_MAX_REQUESTS)
    {
        LOG(ERROR) << "Reached maximum concurrent requests";
        return;     
    }
    else if (!channel_list[channel])
    {
        return;
    }
    else if ( (channel_list.find(channel) != channel_list.end()) &&  // channel found
              (channel_list[channel]->value != value))               // value changed
    {
        LOG(DEBUG) << "Spawning request for channel #" << channel << " with val: " << value << " prev val: " << channel_list[channel]->value;
        channel_list[channel]->value = value;
        
        HTTPRequestHandler *request = new HTTPRequestHandler(channel_list[channel]);
        active_requests.push_back(request);
    }
}

//Set a hardware channel output. For now, data only gets sent if value changes.
int HTTPRequestDevice::getChannelCount()
{
    return channel_list.size();
}

// Loop over list of active channels, removing those that have finished
void HTTPRequestDevice::updateLoop()
{    
    std::list<HTTPRequestHandler*>::iterator it;
    
    while (run_thread)
    {
        if ( (!enabled) && (start_time.getElapsedTime().asSeconds() > 15) )
            enabled = true;
            
        // Loop over vector of requests, and close each ready request-object
        for (it = active_requests.begin(); it != active_requests.end(); it++)
        {
            if ((*it)->isReady()) //remove request-object if it has finished
            {
                //delete * it;
                it = active_requests.erase(it);
            }
        }
        sf::sleep(refresh_interval);
    }
    
    // Wait for each request to have finished
    sf::Clock timer;
    while ((active_requests.size() > 0) &&
           (timer.getElapsedTime().asSeconds() < timeout))
    {
        sf::sleep(sf::milliseconds(100));
    }
    // Destroy all requests
    active_requests.clear();

}
