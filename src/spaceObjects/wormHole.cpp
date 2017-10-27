#include <SFML/OpenGL.hpp>

#include "main.h"
#include "wormHole.h"
#include "spaceship.h"
#include "scriptInterface.h"

#define FORCE_MULTIPLIER          50.0
#define FORCE_MAX                 10000.0
#define ALPHA_MULTIPLIER          10.0
#define DEFAULT_COLLISION_RADIUS  2500
#define AVOIDANCE_MULTIPLIER      1.2
#define TARGET_SPREAD             500

// A wormhole object that drags objects toward it like a black hole, and then
// teleports them to another point when they reach its center.
REGISTER_SCRIPT_SUBCLASS(WormHole, SpaceObject)
{
    /// Set the target of this wormhole
    REGISTER_SCRIPT_CLASS_FUNCTION(WormHole, setTargetPosition);
}

REGISTER_MULTIPLAYER_CLASS(WormHole, "WormHole");
WormHole::WormHole()
: SpaceObject(DEFAULT_COLLISION_RADIUS, "WormHole") 
{
    pathPlanner = PathPlannerManager::getInstance();
    pathPlanner->addAvoidObject(this, (DEFAULT_COLLISION_RADIUS * AVOIDANCE_MULTIPLIER) );
    
    setRadarSignatureInfo(0.9, 0.0, 0.0);

    // Choose a texture to show on radar
    radar_visual = irandom(1, 3);
    registerMemberReplication(&radar_visual);
    
    // Create some overlaying clouds
    for(int n=0; n<cloud_count; n++)
    {
        clouds[n].size = random(1024, 1024 * 4);
        clouds[n].texture = irandom(1, 3);
        clouds[n].offset = sf::Vector2f(0, 0); 
    }
}

#if 0 //SEVCOL
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

        ShaderManager::getShader("billboardShader")->setParameter("textureMap", *textureManager.getTexture("wormHole" + string(cloud.texture) + ".png"));
        sf::Shader::bind(ShaderManager::getShader("billboardShader"));
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
#endif //SEVCOL

void WormHole::draw3D()
{
    //glTranslatef(0, 0, z);
    //glRotatef(engine->getElapsedTime() * rotation_speed, 0, 0, 1);
    glScalef(2, 2, 2);

    sf::Shader* shader = ShaderManager::getShader("basicShader");
    shader->setParameter("baseMap", *textureManager.getTexture("black.png"));
    // shader->setParameter("specularMap", *textureManager.getTexture("Astroid_0_s.png"));
    sf::Shader::bind(shader);
    Mesh* m = Mesh::getMesh("gate2.obj");
    m->render();
}


void WormHole::drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{
    float distance = sqrt( pow( (position.x - getPosition().x), 2) +
                           pow( (position.y - getPosition().y), 2) );

    if ( distance < 3000)
    {
        int transparency;
        if (distance > 1500)
        {
            // Make transparent is still far away
            transparency = (int) 255 * ( (3000-distance) / 1500);
        }
        else
        {
            transparency = 255;
        }
        sf::Sprite object_sprite;
        textureManager.setTexture(object_sprite, "wormHole" + string(radar_visual) + ".png");
        object_sprite.setRotation(getRotation());
        object_sprite.setPosition(position);
        float size = getRadius() * scale / object_sprite.getTextureRect().width;
        object_sprite.setScale(size, size);
        object_sprite.setColor(sf::Color(255, 255, 255, transparency));
        window.draw(object_sprite, sf::BlendAdd);

        if (distance < 2000)
        {
            sf::CircleShape range_circle(getRadius() * scale);
            range_circle.setOrigin(getRadius() * scale, getRadius() * scale);
            range_circle.setPosition(position);
            range_circle.setFillColor(sf::Color::Transparent);
            range_circle.setOutlineColor(sf::Color(255, 255, 255, 16));
            range_circle.setOutlineThickness(2.0);
            window.draw(range_circle);
        }
    }
}

/* Draw a line towards the target position */
void WormHole::drawOnGMRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range)
{    
    sf::Sprite object_sprite;
    textureManager.setTexture(object_sprite, "wormHole" + string(radar_visual) + ".png");
    object_sprite.setRotation(getRotation());
    object_sprite.setPosition(position);
    float size = getRadius() * scale / object_sprite.getTextureRect().width;
    object_sprite.setScale(size, size);
    object_sprite.setColor(sf::Color(255, 255, 255));
    window.draw(object_sprite, sf::RenderStates(sf::BlendAdd));


    sf::VertexArray a(sf::Lines, 2);
    a[0].position = position;
    a[1].position = position + (target_position - getPosition()) * scale;
    a[0].color = sf::Color(255, 255, 255, 32);
    window.draw(a);
    
    sf::CircleShape range_circle(getRadius() * scale);
    range_circle.setOrigin(getRadius() * scale, getRadius() * scale);
    range_circle.setPosition(position);
    range_circle.setFillColor(sf::Color::Transparent);
    range_circle.setOutlineColor(sf::Color(255, 255, 255, 32));
    range_circle.setOutlineThickness(2.0);
    window.draw(range_circle);
}


void WormHole::update(float delta)
{
    update_delta = delta;
}

void WormHole::collide(Collisionable* target, float collision_force)
{
    if (update_delta == 0.0 || !is_active)
        return;

    sf::Vector2f diff = getPosition() - target->getPosition();
    float distance = sf::length(diff);
    float force = (getRadius() * getRadius() * FORCE_MULTIPLIER) / (distance * distance);
    
    P<SpaceShip> obj = P<Collisionable>(target);

    // Warp postprocessor-alpha is calculated using alpha = (1 - (delay/10))
    if (obj)
        obj->wormhole_alpha = ((distance / getRadius()) * ALPHA_MULTIPLIER);
    
    // TODO: Escaping is impossible. Change setPosition to something Newtonianish.
    target->setPosition(target->getPosition() + diff / distance * update_delta * force);

    if (force > FORCE_MAX)
    {
        force = FORCE_MAX;
        if (isServer())
        {
            target->setPosition( (target_position +
                                  sf::Vector2f(random(-TARGET_SPREAD, TARGET_SPREAD),
                                               random(-TARGET_SPREAD, TARGET_SPREAD))));
            if (obj)
                obj->wormhole_alpha = 0.0;
        }
    }
}

void WormHole::setTargetPosition(sf::Vector2f v)
{
    target_position = v;
}
