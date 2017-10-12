// Copyright(c) 2017 Happy Rock Studios

#ifndef ZBOINGER_H
#define ZBOINGER_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;

class ZBoinger : public jl::Prop {
public:

    typedef fwk::Ptr<ZBoinger> Ptr;

    ZBoinger(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, float strength = 100, bool dynamic = false);

    void readySet();

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds() {}

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete() {}

    float strength() const { return _strength; }

    void handleInput(std::string input);

private:
    float _strength;
};

#endif
