#ifndef WORMHOLE_H
#define WORMHOLE_H

#include "spaceObject.h"
#include "pathPlanner.h"

class WormHole : public SpaceObject, public Updatable
{
    sf::Vector2f target_position = sf::Vector2f(0.0f, 0.0f);
    float update_delta = 0.0f;
    P<PathPlannerManager>  pathPlanner;

public:

    WormHole();

    virtual void drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range);
    virtual void drawOnGMRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range);
    virtual void update(float delta);
    virtual void collide(Collisionable* target);
    
    void setTargetPosition(sf::Vector2f v);   /* Where to jump to */
};

#endif//WORMHOLE_H

