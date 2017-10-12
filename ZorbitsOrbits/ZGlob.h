// Copyright(c) 2017 Happy Rock Studios

#ifndef ZGLOB_H
#define ZGLOB_H

#include <sstream>

#include "JollyLava/Character.h"

class ZorbitLevel;

class ZGlob : public jl::Character {
public:

    typedef fwk::Ptr<ZGlob> Ptr;

    ZGlob(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void reset();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    //ZGlob specific movements
    void attack();

    void move(std::string direction);

    std::string direction();

private:
    /// Is Zorbit in sight?
    bool _leftSensor;
    bool _rightSensor;

    int _lFootContacts;
    int _rFootContacts;

};

#endif


