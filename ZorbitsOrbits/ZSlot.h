// Copyright(c) 2017 Happy Rock Studios

#ifndef ZSLOT_H
#define ZSLOT_H

#include <sstream>

#include "JollyLava/Prop.h"

class ZorbitLevel;

class ZSlot : public jl::Prop {
public:

    typedef fwk::Ptr<ZSlot> Ptr;

    ZSlot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void readySet();

    void reset();

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void draw();

    void activate();

    void setSprites(std::string result);

    void reward(std::string result);

    std::string generateResult();

    std::string intToSlotResult(int i);

private:
    bool _active;

    jl::TextureDesc::Ptr _iconTextureDesc;

    sf::Sprite _slot1Sprite;
    sf::Sprite _slot2Sprite;
    sf::Sprite _slot3Sprite;

    sftools::Chronometer _spinClock;
};

#endif

