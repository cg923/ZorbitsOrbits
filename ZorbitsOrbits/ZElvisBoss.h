// Copyright(c) 2017 Happy Rock Studios

#ifndef ZELVISBOSS_H
#define ZELVISBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyLevels
#include "ZBoss.h"

class ZorbitLevel;

class ZElvisBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZElvisBoss> Ptr;

    ZElvisBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void reset();

    void orientTowardGravityFieldCenter();

    //ZElvisBoss specific movements
    void move(std::string direction);

    void stop();

    void tailWhip();

    void shoot();

    std::string direction();

    bool stunning() const { return _stunning; }

    void groundSmoke();

    void phase1(bool right);

    void phase2(bool right);

    void phase3(bool right);

    void limitIs(bool input) {_limit = input; }

    void roar();

    void changeToDeathTexture();

private:
    int                             _numShots;
    sftools::Chronometer           _shotTimer;
    std::stringstream               _shotName;
    sftools::Chronometer           _actionTimer;

    bool        _stunning;
    sftools::Chronometer            _stunClock;
    int         _timesOnGround;

    bool        _tailHit;
    bool        _shooting;
    bool        _limit;

    bool        _phase2;
    bool        _phase3;

    int         SHOOT_RATE;

};

class ebAttackAction : public jl::Action
{
    public:
        ebAttackAction(jl::Actor * actor) : jl::Action("TailWhip", true, 1, actor), _hasAttacked(false) {}

        void reset()
        {
            _hasAttacked = false;
            Action::reset();
        }

        void update(bool animFinished)
        {
            if(!_hasAttacked)
            {
                static_cast<ZElvisBoss *>(actor())->tailWhip();
                _hasAttacked = true;
            }
            jl::Action::update(animFinished);
        }
    private:
        bool    _hasAttacked;
};

class EBShot : public jl::Projectile {
public:

    typedef fwk::Ptr<EBShot> Ptr;

    EBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 1.0f);

    EBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 1.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();

private:
    sftools::Chronometer _fadeTimer;
};

class ebShotMoveAction : public jl::Action
{
    public:
        ebShotMoveAction(EBShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

class EBShot2 : public jl::Projectile {
public:

    typedef fwk::Ptr<EBShot2> Ptr;

    EBShot2(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 10.0f);

    EBShot2(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 10.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();

private:
    int _frame;
};

class ebShot2MoveAction : public jl::Action
{
    public:
        ebShot2MoveAction(EBShot2 * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif



