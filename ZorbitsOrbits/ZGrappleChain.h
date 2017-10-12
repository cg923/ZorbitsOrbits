// Copyright(c) 2017 Happy Rock Studios

#ifndef ZGRAPPLECHAIN_H_INCLUDED
#define ZGRAPPLECHAIN_H_INCLUDED

#include <sstream>

#include "JollyLava/Entity.h"

class ZorbitLevel;

class GrappleChain : public jl::Entity
{
public:

    typedef fwk::Ptr<GrappleChain> Ptr;

    GrappleChain(jl::Entity * zorbit, jl::Level * level, std::vector<string> layerNames, b2Vec2 initPos);

    void attach(jl::Entity * target);

    void detach();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD) {}

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD) {}

    float cameraAngle() { return 0; }

    void defineAnimations();

    void defineSounds() {}

    void updateConcrete();

    void activeIs(bool input) { _active = input; }

    bool active() { return _active; }

private:

    sftools::Chronometer _visibilityClock;

    int  _linkCount;
    bool _active;

    Entity * _zorbit;
    Entity * _grapplePoint;
};

#endif




