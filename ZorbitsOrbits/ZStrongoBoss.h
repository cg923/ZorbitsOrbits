// Copyright(c) 2017 Happy Rock Studios

#ifndef ZSTRONGOBOSS_H
#define ZSTRONGOBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyLevels
#include "ZBoss.h"
#include "ZCircle.h"

class ZorbitLevel;

enum Trash
{
    tire,
    gear,
    bear,
    trump
};

class ZStrongoBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZStrongoBoss> Ptr;

    ZStrongoBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void reset();

    //ZStrongoBoss specific movements
    void move(std::string direction);

    void stop();

    void attack();

    void shoot();

    void pound();

    std::string direction();

    void phase1(bool right);

    void phase2(bool right);

    void phase3(bool right);

    void cleanUp();

private:
    int                 _numShots;
    std::stringstream   _shotName;
    sftools::Chronometer     _shotTimer;
    sftools::Chronometer     _actionTimer;
    sftools::Chronometer     _poundTimer;
    sftools::Chronometer     _poundTimer2;
    int                 _minShotIntervalInSeconds;

    std::list<std::string>   _trash;

    bool        _destroyingJoints;
    bool        _intro;
    bool        _phase2;
    bool        _phase3;

    b2RevoluteJoint * _throwerJoint;
    b2RevoluteJoint * _footJoint;

    void destroyJoints();
};

class sbAttackAction : public jl::Action
{
    public:
        sbAttackAction(jl::Actor * actor) : jl::Action("Attack", true, 1, actor), _hasAttacked(false) {}

        void reset()
        {
            _hasAttacked = false;
            Action::reset();
        }

        void update(bool animFinished)
        {
            if(!_hasAttacked)
            {
                static_cast<ZStrongoBoss *>(actor())->attack();
                _hasAttacked = true;
            }
            jl::Action::update(animFinished);
        }
    private:
        bool    _hasAttacked;
};

class sbPoundAction : public jl::Action
{
    public:
        sbPoundAction(jl::Actor * actor, int loops = 1) : jl::Action("Pound", true, loops, actor) {}

        void update(bool animFinished)
        {
            static_cast<ZStrongoBoss *>(actor())->pound();

            jl::Action::update(animFinished);
        }
    private:
        //bool    _hasAttacked;
};

class SBTrash : public ZCircle {
public:

    typedef fwk::Ptr<SBTrash> Ptr;

    SBTrash(std::string name,
            ZorbitLevel * level,
            std::vector<string> layerNames,
            int whichTrash,
            std::string direction,
            b2Vec2 initPos,
            float radius = 2.0f,
            float duration = 4.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void breakJoints();

    void setHoldTime(float input) { _holdTime = input; }

    void setThrowPower(float input) { _throwPower = input; }

    float cameraAngle() { return angle(); }

private:
    sftools::Chronometer   _fadeTimer;
    sftools::Chronometer   _timer;
    float _duration;
    float _holdTime;
    float _throwPower;
    std::string _direction;

    bool _canHurtZorbit;
    bool _fading;
    bool _destroyJoint;
    bool _breakJoint;

    b2WeldJoint * _positionJoint;
};

class SBShot : public jl::Projectile {
public:

    typedef fwk::Ptr<SBShot> Ptr;

    SBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 1.0f);

    SBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 1.0f);

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

class sbShotMoveAction : public jl::Action
{
    public:
        sbShotMoveAction(SBShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif



