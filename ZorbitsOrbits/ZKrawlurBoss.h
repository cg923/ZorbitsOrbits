
// Copyright(c) 2017 Happy Rock Studios

#ifndef ZKRAWLURBOSS_H
#define ZKRAWLURBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyLevels
#include "ZBoss.h"

class ZorbitLevel;

class ZKrawlurBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZKrawlurBoss> Ptr;

    ZKrawlurBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void activate();

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void reset();

    //ZKrawlurBoss specific movements
    void move(std::string direction);

    void jump(float height);

    void stop();

    void attack();

    void randomDirection();

    std::string direction();

    void phase1(bool right);

    void phase2(bool right);

    void phase3(bool right);

    void cleanUp();

    void destroyJoints();

private:
    int                         _numShots;
    int                         _maxShotsTracker;
    int                         MAXSHOTS;
    std::stringstream           _shotName;
    sftools::Chronometer        _shotTimer;
    sftools::Chronometer        _actionTimer;
    float                       _minShotIntervalInSeconds;

    bool                        _leftTree;
    bool                        _rightTree;
    bool                        _ground;

    bool                        _phase2;
    bool                        _jumped;
    bool                        _shooting;
    bool                        _phase3;

    b2RevoluteJoint *           _gunJoint;
    b2RevoluteJoint *           _footJoint;
};

class kbAttackAction : public jl::Action
{
    public:
        kbAttackAction(jl::Actor * actor) : jl::Action("Attack", true, 1, actor) {}

        void update(bool animFinished)
        {
            static_cast<ZKrawlurBoss *>(actor())->attack();

            jl::Action::update(animFinished);
        }
};

class kbJumpAction : public jl::Action
{
    public:
        kbJumpAction(ZKrawlurBoss * krawlur) : Action("Jump", false, 1, krawlur)
        {
            _height = static_cast<ZKrawlurBoss*>(actor())->jumpHeight();
            _jumping = false;
        }

        void giveValue(std::string value)
        {
            if(atoi(value.c_str()) > 0)
                _height = atoi(value.c_str());
        }

        void update(bool animFinished)
        {
            if(!_jumping)
            {
                _jumpClock.reset(true);
                _jumping = true;
                _height = static_cast<ZKrawlurBoss*>(actor())->jumpHeight();
                static_cast<ZKrawlurBoss*>(actor())->jump(_height);
            }

            if(_jumping
               && _jumpClock.getElapsedTime().asMilliseconds() > 250)
            {
                jumpReset();
            }

            Action::update(animFinished);
        }
        void jumpReset()
        {
            _jumping = false;
            Action::reset();
            actor()->actionMan()->endAction("Jump");
        }

    private:
        bool        _jumping;
        float       _height;
        sftools::Chronometer   _jumpClock;
};

class KBShot : public jl::Projectile {
public:

    typedef fwk::Ptr<KBShot> Ptr;

    KBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 30.0f);

    KBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 30.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();
};

class kbShotMoveAction : public jl::Action
{
    public:
        kbShotMoveAction(KBShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};


#endif



