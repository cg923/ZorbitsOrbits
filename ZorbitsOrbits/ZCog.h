// Copyright(c) 2017 Happy Rock Studios

#ifndef ZCOG_H
#define ZCOG_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;

class ZCog : public jl::Prop {
public:

    typedef fwk::Ptr<ZCog> Ptr;

    ZCog(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds() {}

    void readySet();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete() {}

    float cameraAngle() const { return rootBody()->b2body()->GetAngle(); }

    void deactivate();

    void activate();

    bool active() const { return _active; }

private:
    bool _active;
};

#endif


