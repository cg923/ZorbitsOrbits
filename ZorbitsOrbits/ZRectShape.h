
// Copyright(c) 2017 Happy Rock Studios

#ifndef ZRECTSHAPE_H
#define ZRECTSHAPE_H

#include <sstream>

#include "JollyLava/Actor.h"

class ZorbitLevel;

class ZRectShape : public jl::Actor {
public:

    typedef fwk::Ptr<ZRectShape> Ptr;

    ZRectShape(std::string name,
                    ZorbitLevel * level,
                    std::vector<string> layerNames,
                    b2Vec2 initPos,
                    int initAngle,
                    float width,
                    float height,
                    std::string filepath);

    void readySet();

    void reset();

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineActions();

    void defineSounds() {}

    float cameraAngle();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void update();

    void updateConcrete();

    void move(std::string direction) {}

    void stop() {}

private:

    sf::FloatRect _initRect;
};

#endif


