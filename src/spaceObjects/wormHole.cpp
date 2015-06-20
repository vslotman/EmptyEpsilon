#include "main.h"
#include "wormHole.h"
#include "spaceship.h"
#include "scriptInterface.h"

#define FORCE_MULTIPLIER          50.0
#define FORCE_MAX                 10000.0
#define ALPHA_MULTIPLIER          10.0
#define DEFAULT_COLLISION_RADIUS  5000
#define AVOIDANCE_MULTIPLIER      1.2

/// A Wormhole object. Drags you in like a black hole and teleports when you're at the center.
REGISTER_SCRIPT_SUBCLASS(WormHole, SpaceObject)
{
}

REGISTER_MULTIPLAYER_CLASS(WormHole, "WormHole");
WormHole::WormHole()
: SpaceObject(DEFAULT_COLLISION_RADIUS, "WormHole")
{
    pathPlanner = PathPlannerManager::getInstance();
    pathPlanner->addAvoidObject(this, (DEFAULT_COLLISION_RADIUS * AVOIDANCE_MULTIPLIER) );
}

void WormHole::drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{
    sf::Sprite objectSprite;
    textureManager.setTexture(objectSprite, "icon_wormhole.png");
    objectSprite.setRotation(getRotation());
    objectSprite.setPosition(position);
    objectSprite.setScale(0.8, 0.8);
    window.draw(objectSprite);
}

void WormHole::drawOnGMRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{    
    sf::VertexArray a(sf::Lines, 2);
    a[0].position = position;
    a[1].position = position + (target_position - getPosition()) * scale;
    a[0].color = sf::Color(255, 255, 255, 32);
    window.draw(a);
}


void WormHole::update(float delta)
{
    update_delta = delta;
}

void WormHole::collide(Collisionable* target)
{
    if (update_delta == 0.0)
    return;

    sf::Vector2f diff = getPosition() - target->getPosition();
    float distance = sf::length(diff);
    float force = (getRadius() * getRadius() * FORCE_MULTIPLIER) / (distance * distance);
    P<SpaceShip> obj = P<Collisionable>(target);
    
    if (force > FORCE_MAX)
    {
        force = FORCE_MAX;
        if (isServer())
            target->setPosition(target_position);
            obj->wormhole_alpha = 0.0;
    }
    
    // warp postprocessor-alpha is calculated using alpha = (1 - (delay/10))
    obj->wormhole_alpha = ((distance / getRadius()) * ALPHA_MULTIPLIER);
    
    //TODO Change setPosition to something Newtonianish. Now escaping is impossible
    target->setPosition(target->getPosition() + diff / distance * update_delta * force);
}

void WormHole::setTargetPosition(sf::Vector2f v)
{
    target_position = v;
}
