// Copyright(c) 2017 Happy Rock Studios

#ifndef ZBOOSTER_H
#define ZBOOSTER_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;

class ZBooster : public jl::Prop {
public:

    typedef fwk::Ptr<ZBooster> Ptr;

    ZBooster(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, double strength = 55.0);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds() {}

    void readySet();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete() {}

    float cameraAngle() const { return rootBody()->b2body()->GetAngle(); }

    double boostStrength() const { return _boostStrength; }

private:
    double _boostStrength;
};

#endif

