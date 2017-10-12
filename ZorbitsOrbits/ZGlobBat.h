// Copyright(c) 2017 Happy Rock Studios

#ifndef ZGLOBBAT_H
#define ZGLOBBAT_H

#include <sstream>

#include "JollyLava/Character.h"

class ZorbitLevel;

class ZGlobBat : public jl::Character {
public:

    typedef fwk::Ptr<ZGlobBat> Ptr;

    ZGlobBat(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void readySet();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void destroyJoints();

    //ZGlobBat specific movements
    void move(std::string direction);

    void die();

    void switchDirection();

    std::string direction();

private:

    b2DistanceJoint * _distanceJoint;
    bool _jointDestroyed;

};

class gbMRightAction : public jl::Action
{
    public:
        gbMRightAction(jl::Actor * actor, std::string name = "MoveRight") : jl::Action(name, false, 0, actor){ }
        gbMRightAction(float loops, jl::Actor * actor, std::string name = "MoveRight") : jl::Action(name, true, loops, actor){ }

        void setLoops(std::string value)
        {
            _initLoops = atoi(value.c_str());
            _terminates = true;
        }

        void update(bool animFinished);
};

class gbMLeftAction : public jl::Action
{
    public:
        gbMLeftAction(jl::Actor * actor, std::string name = "MoveLeft") : jl::Action(name, false, 0, actor){ }
        gbMLeftAction(float loops, jl::Actor * actor, std::string name = "MoveLeft") : jl::Action(name, true, loops, actor){ }

        void setLoops(std::string value)
        {
            _initLoops = atoi(value.c_str());
            _terminates = true;
        }

        void update(bool animFinished);
};

#endif



