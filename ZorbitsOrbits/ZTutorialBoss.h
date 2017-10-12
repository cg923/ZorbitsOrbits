// Copyright(c) 2017 Happy Rock Studios

#ifndef ZTUTORIALBOSS_H
#define ZTUTORIALBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyLevels
#include "ZBoss.h"

class ZorbitLevel;

class ZTutorialBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZTutorialBoss> Ptr;

    ZTutorialBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void reset();

    //ZTutorialBoss specific movements
    void attack(std::string direction = "Left");

    void moveRandom();

    std::string direction();

private:
    int _numShots;

};

class tbLAttackAction : public jl::Action
{
    public:
        tbLAttackAction(jl::Actor * actor) : jl::Action("AttackLeft", true, 1, actor), _hasAttacked(false) {}

        void reset()
        {
            _hasAttacked = false;
            Action::reset();
        }

        void update(bool animFinished)
        {
            if(!_hasAttacked)
            {
                static_cast<ZTutorialBoss *>(actor())->attack("Left");
                _hasAttacked = true;
            }
            jl::Action::update(animFinished);
        }
    private:
        bool    _hasAttacked;
};

class tbRAttackAction : public jl::Action
{
    public:
        tbRAttackAction(jl::Actor * actor) : jl::Action("AttackRight", true, 1, actor), _hasAttacked(false) { }

        void reset()
        {
            _hasAttacked = false;
            Action::reset();
        }

        void update(bool animFinished)
        {
            if(!_hasAttacked)
            {
                static_cast<ZTutorialBoss *>(actor())->attack("Right");
                _hasAttacked = true;
            }
            jl::Action::update(animFinished);
        }
    private:
        bool    _hasAttacked;
};

class TBShot : public jl::Projectile {
public:

    typedef fwk::Ptr<TBShot> Ptr;

    TBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 3.0f);

    TBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 3.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();
};

class tbShotMoveAction : public jl::Action
{
    public:
        tbShotMoveAction(TBShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif



