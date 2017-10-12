// Copyright(c) 2017 Happy Rock Studios

#ifndef ZGRAPPLE_H
#define ZGRAPPLE_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;
class ZCircle;

class ZGrapple : public jl::Prop {
public:

    typedef fwk::Ptr<ZGrapple> Ptr;

    ZGrapple(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, bool dynamic = false);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds() {}

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void giveZCircle(ZCircle* circle) { _zcircle = circle;
                                        _hasZCircle = true;}

    bool hasZCircle() const { return _hasZCircle; }

    ZCircle* zCircle() { return _zcircle; }

    // ZGrapple specific
    void activate() { _activated = true; }

    void deactivate() { _activated = false; }

private:
    bool _activated;

    ZCircle* _zcircle;
    bool    _hasZCircle;
};

#endif
