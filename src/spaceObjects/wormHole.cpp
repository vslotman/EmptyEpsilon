#include <SFML/OpenGL.hpp>

#include "main.h"
#include "wormHole.h"
#include "spaceship.h"
#include "scriptInterface.h"

#define FORCE_MULTIPLIER          50.0
#define FORCE_MAX                 10000.0
#define ALPHA_MULTIPLIER          10.0
#define DEFAULT_COLLISION_RADIUS  5000
#define AVOIDANCE_MULTIPLIER      1.2
#define TARGET_SPREAD             500

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
    
    // Create some overlaying clouds
    for(int n=0; n<cloud_count; n++)
    {
        clouds[n].size = random(1024, 1024 * 4);
        clouds[n].texture = irandom(1, 3);
        clouds[n].offset = sf::Vector2f(0, 0); 
    }
}

#if FEATURE_3D_RENDERING
void WormHole::draw3DTransparent()
{
    glRotatef(getRotation(), 0, 0, -1);
    glTranslatef(-getPosition().x, -getPosition().y, 0);
    for(int n=0; n<cloud_count; n++)
    {
        NebulaCloud& cloud = clouds[n];

        sf::Vector3f position = sf::Vector3f(getPosition().x, getPosition().y, 0) + sf::Vector3f(cloud.offset.x, cloud.offset.y, 0);
        float size = cloud.size;
        
        float distance = sf::length(camera_position - position);
        float alpha = 1.0 - (distance / 10000.0f);
        if (alpha < 0.0)
            continue;

        billboardShader->setParameter("textureMap", *textureManager.getTexture("Nebula" + string(cloud.texture) + ".png"));
        sf::Shader::bind(billboardShader);
        glBegin(GL_QUADS);
        glColor4f(alpha * 0.8, alpha * 0.8, alpha * 0.8, size);
        glTexCoord2f(0, 0);
        glVertex3f(position.x, position.y, position.z);
        glTexCoord2f(1, 0);
        glVertex3f(position.x, position.y, position.z);
        glTexCoord2f(1, 1);
        glVertex3f(position.x, position.y, position.z);
        glTexCoord2f(0, 1);
        glVertex3f(position.x, position.y, position.z);
        glEnd();
    }
}
#endif//FEATURE_3D_RENDERING


void WormHole::drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{
    sf::Sprite objectSprite;
    textureManager.setTexture(objectSprite, "icon_wormhole.png");
    objectSprite.setRotation(getRotation());
    objectSprite.setPosition(position);
    objectSprite.setScale(0.8, 0.8);
    window.draw(objectSprite);
}

/* Draw a line towards the target position */
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
            target->setPosition( (target_position + 
                                  sf::Vector2f(random(-TARGET_SPREAD, TARGET_SPREAD), 
                                               random(-TARGET_SPREAD, TARGET_SPREAD))));
            if (obj)
                obj->wormhole_alpha = 0.0;
    }
    
    // warp postprocessor-alpha is calculated using alpha = (1 - (delay/10))
    if (obj)
        obj->wormhole_alpha = ((distance / getRadius()) * ALPHA_MULTIPLIER);
    
    //TODO Change setPosition to something Newtonianish. Now escaping is impossible
    target->setPosition(target->getPosition() + diff / distance * update_delta * force);
}

void WormHole::setTargetPosition(sf::Vector2f v)
{
    target_position = v;
}
