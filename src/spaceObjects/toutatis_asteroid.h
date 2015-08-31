#ifndef TOUTATIS_ASTEROID_H
#define TOUTATIS_ASTEROID_H

#include "spaceObject.h"

class ToutatisAsteroid : public SpaceObject
{
public:
    float rotation_speed;
    float z;

    ToutatisAsteroid();

    virtual void draw3D();

    virtual void drawOnRadar(sf::RenderTarget& window, sf::Vector2f position, float scale, bool long_range);

    virtual void collide(Collisionable* target);
    
    virtual string getExportLine() { return "ToutatisAsteroid():setPosition(" + string(getPosition().x, 0) + ", " + string(getPosition().y, 0) + ")"; }
};

class VisualToutatisAsteroid : public SpaceObject
{
public:
    float rotation_speed;
    float z;

    VisualToutatisAsteroid();

    virtual void draw3D();

    virtual string getExportLine() { return "VisualToutatisAsteroid():setPosition(" + string(getPosition().x, 0) + ", " + string(getPosition().y, 0) + ")"; }
};

#endif//TOUTATIS_ASTEROID_H
