
// Copyright(c) 2017 Happy Rock Studios

#ifndef ZVEXBOSS_H
#define ZVEXBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyLevels
#include "ZBoss.h"

class ZorbitLevel;

class ZVexBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZVexBoss> Ptr;

    ZVexBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void readySet();

    void destroyJoints();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    std::string direction();

    void reset();

    //ZVexBoss specific movements
    void phase1();

    void phase2();

    void phase3();

    void phase4();

    void shoot();

    void stop();

    void move(std::string direction);

    void enableWheel();

    void disableWheel();

    void blink();

    void unblink();

    void hyperSpin();

    void stopHyperSpin();

private:
    bool                _vulnerable;
    bool                _shoot;
    bool                _phase2;
    bool                _phase3;
    bool                _phase4;
    bool                _blink;

    b2DistanceJoint *   _distanceJoint;
    bool                _distanceJointOn;
    b2WheelJoint *      _wheelJoint;
    float               _wheelRadius;
    b2RevoluteJoint *   _gunLeftJoint;
    b2RevoluteJoint *   _gunRightJoint;

    sftools::Chronometer _actionTimer;
    sftools::Chronometer _blinkTimer;
    sftools::Chronometer _shotTimer;
    int _numShots;
    std::stringstream _shotName;
    std::string     _shotDirection;
};

class vStopAction : public jl::Action
{
    public:
        vStopAction(ZVexBoss * vex) : Action("Stop", false, 0, vex){}

        vStopAction(float duration, ZVexBoss * vex) : Action("Stop", true, duration, vex){}

        void update(bool animFinished)
        {
            static_cast<ZVexBoss *>(actor())->stop();

            Action::update(animFinished);
        }
};

class vHyperSpinAction : public jl::Action
{
    public:
        vHyperSpinAction(ZVexBoss * vex) : Action("HyperSpin", false, 0, vex){}

        vHyperSpinAction(float duration, ZVexBoss * vex) : Action("Hyper", true, duration, vex){}

        void update(bool animFinished)
        {
            static_cast<ZVexBoss*>(actor())->hyperSpin();

            Action::update(animFinished);
        }
};

// --------------------
// VSHOT
// ---------------------

class VShot : public jl::Projectile {
public:

    typedef fwk::Ptr<VShot> Ptr;

    VShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 2.0f);

    VShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 2.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();
};

class vbShotMoveAction : public jl::Action
{
    public:
        vbShotMoveAction(VShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif




