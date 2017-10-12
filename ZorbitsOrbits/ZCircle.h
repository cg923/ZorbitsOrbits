// Copyright(c) 2017 Happy Rock Studios

#ifndef ZCIRCLE_H
#define ZCIRCLE_H

#include <sstream>

#include "JollyLava/Actor.h"

class ZorbitLevel;

class ZCircle : public jl::Actor {
public:

    typedef fwk::Ptr<ZCircle> Ptr;

    ZCircle(std::string name,
            ZorbitLevel * level,
            std::vector<string> layerNames,
            std::string texturePath,
            b2Vec2 initPos,
            int initAngle,
            float radius,
            std::string fixed,
            bool resizeSprite = false);

    void readySet();

    void reset();

    void update();

    void roll();

    void move(std::string direction) {}

    void stop() {}

    void switchSpinDirection();

    void changeSpinSpeed(float factor);

    bool spinning() const { return _spinning; }

    void setMass(float mass);

    void setDensity(float density);

    void fade();

    void setSensor(bool sensor);

    bool isSensor() const { return _sensor; }

    void disableLimits();

    void defineLimits(float lower, float upper);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle();

private:

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineActions();

    void defineSounds() {}

    void makeConnections();

    float _radius;
    bool _fixed;
    bool _sensor;

    std::map<std::string, Collidable::Ptr> _connections;
    std::map<std::string, Collidable::Ptr> _pendingConnections;
    std::map<std::string, b2Joint*> _joints;

    b2RevoluteJoint * _revoluteJoint;
    b2DistanceJoint * _distanceJoint;

    float   _lastAngle;
    bool    _spinning;
    float   _initSpinSpeed;

    bool    _fade;
    sftools::Chronometer _fadeTimer;
};

#endif

