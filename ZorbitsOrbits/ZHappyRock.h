// Copyright(c) 2017 Happy Rock Studios

#ifndef ZHAPPYROCK_H
#define ZHAPPYROCK_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;

class ZHappyRock : public jl::Prop {
public:

    typedef fwk::Ptr<ZHappyRock> Ptr;

    ZHappyRock(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, float strength = 100, bool dynamic = false);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void toDie() { _toDie = true; }

private:
    bool _toDie;
};

#endif

