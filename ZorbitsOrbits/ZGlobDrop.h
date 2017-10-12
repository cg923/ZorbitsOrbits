// Copyright(c) 2017 Happy Rock Studios

#ifndef ZGLOBDROP_H
#define ZGLOBDROP_H

#include <sstream>

#include "JollyLava/Character.h"
#include "JollyLava/Projectile.h"

class ZorbitLevel;

class ZGlobDrop : public jl::Character {
public:

    typedef fwk::Ptr<ZGlobDrop> Ptr;

    ZGlobDrop(std::string name,
              ZorbitLevel * level,
              std::vector<string> layerNames,
              b2Vec2 initPos,
              int initAngle,
              bool sideways = false);

    void defineFixtures(float width, float height);

    void defineAnimations() {}

    void defineAnimations(bool sideways);

    void defineSounds();

    void defineActions();

    void reset();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    //ZGlobDrop specific movements
    void attack();

private:
    int                 _dropCount;
    bool                _dropped;

};

class gdAttackAction : public jl::Action
{
    public:
        gdAttackAction(jl::Actor * actor) : jl::Action("Attack", false, 0, actor){ }

        void update(bool animFinished)
        {
            static_cast<ZGlobDrop *>(actor())->attack();

            jl::Action::update(animFinished);
        }
};

class Drop : public jl::Projectile {
public:

    typedef fwk::Ptr<Drop> Ptr;

    Drop(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle);

    void reset() {}

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();

    void drop();

};

class dDropAction : public jl::Action
{
    public:
        dDropAction(Drop * drop) : Action("Drop", false, 0, drop) {}

        void update(bool animFinished)
        {
            static_cast<Drop *>(actor())->drop();

            Action::update(animFinished);
        }
};
#endif



