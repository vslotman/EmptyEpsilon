#include "httpRequestHandler.h"
#include "logging.h"
#include "stringImproved.h"

HTTPRequestHandler::HTTPRequestHandler(HTTPChannel * target)
: request_thread(&HTTPRequestHandler::performRequest, this)
{
    target_channel = target;
    request_thread.launch();
}

HTTPRequestHandler::~HTTPRequestHandler()
{
    if (!isReady())
    {
        LOG(WARNING) << "Terminating request for " << target_channel->host << ":" << target_channel->port << "/" << target_channel->uri;
        request_thread.terminate();
        is_ready = false;
    }
}

void HTTPRequestHandler::performRequest()
{    
    // Create a new HTTP client
    sf::Http http;
    sf::Time request_timeout = sf::seconds(target_channel->timeout);

    http.setHost(target_channel->host, target_channel->port);

    sf::Http::Request request(target_channel->uri);
    // Send the request
    sf::Http::Response response = http.sendRequest(request, request_timeout);
    ;
    // Check the status code and display the result
    sf::Http::Response::Status status = response.getStatus();
    
    if (status == sf::Http::Response::Ok)
    {
        LOG(INFO) << response.getBody();
    }
    else
    {
        LOG(ERROR) << "Error " << status;
    }
    is_ready = true;
}

bool HTTPRequestHandler::HTTPRequestHandler::isReady()
{
    return is_ready;
}
