// Copyright(c) 2017 Happy Rock Studios

#ifndef ZSHOT_H
#define ZSHOT_H

#include "JollyLava/Projectile.h"

class ZorbitLevel;

class ZShot : public jl::Projectile {
public:

    typedef fwk::Ptr<ZShot> Ptr;

    ZShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 2.0f);

    ZShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 2.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();

};

class zShotMoveAction : public jl::Action
{
    public:
        zShotMoveAction(ZShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif


