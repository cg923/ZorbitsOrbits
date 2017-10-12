// Copyright(c) 2017 Happy Rock Studios

#ifndef ZPLATFORM_H
#define ZPLATFORM_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;

class ZPlatform : public jl::Prop {
public:

    typedef fwk::Ptr<ZPlatform> Ptr;

    ZPlatform(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, std::string activated);

    void readySet();

    void reset();

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds() {}

    void handleInput(std::string input);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete() {}

    bool activated() const { return _activated; }

    void switchActivation();

    void activationIs(bool input);

private:
    std::string _initialActivation;

    bool _activated;

    bool _touchingZorbit;

    b2FixtureDef _fixtureDef;
};

#endif

