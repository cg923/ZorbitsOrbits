// Copyright(c) 2017 Happy Rock Studios

#ifndef ZFUNZOBOSS_H
#define ZFUNZOBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyLevels
#include "ZBoss.h"

class ZorbitLevel;

enum HandState
{
    Stop,
    Move,
    Smash,
    Clap
};

class FunzoHand
{
public:
    FunzoHand(bool leftHand, jl::TextureDesc::Ptr tex, b2Vec2 gravPosition);

    sf::Sprite sprite() { return _sprite; }

    void update();

    void reset();

    void drop(int amount);

    b2Vec2 b2Position() const { return _position; }

    float b2Angle() const { return _angle; }

    void addAction(HandState action, int loops);

    void moveHand(float radial, float tangential);

    bool actionQueueEmpty() { return _actionQueue.empty(); }

    void lockActions() { _actionsLocked = true; }

    void unlockActions() { _actionsLocked = false; }

    void directedLeftIs(bool value) { _directedLeft = value; }

    bool jumping() const { return _jumping; }

    sf::Sprite      _sprite;

private:
    jl::TextureDesc::Ptr _desc;

    sf::Vector2f    _spritePosition;
    b2Vec2          _position;
    float           _angle;
    b2Vec2          _gravPosition;
    bool            _isLeftHand;
    bool            _directedLeft;
    bool            _jumping;
    HandState       _state;

    int             _stopCounter;
    int             _moveCounter;
    int             _smashCounter;
    int             _clapCounter;
    float           _distance;
    bool            _actionsLocked;

    std::list<std::pair<HandState, int> >   _actionQueue;
};

class ZFunzoBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZFunzoBoss> Ptr;

    ZFunzoBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);
    void defineAnimations();
    void defineSounds();
    void defineActions();

    void readySet();
    void reset();
    void resetHands();

    void destroyJoints();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);
    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();
    void updateHands();
    void handleDie() {}

    void draw();

    std::string direction();

    //ZFunzoBoss specific movements
    void spinHead(std::string direction, float speed = 2.0);
    void stopHeadSpin();

    void shoot();

    void switchWheel();
    void changeWheelSpeed(float value, std::string which = "both");

    void print();

private:
    FunzoHand*          _leftHand;
    FunzoHand*          _rightHand;

    b2RevoluteJoint     * _headJoint;
    bool                _headJointOn;

    sftools::Chronometer _actionTimer;
    int _loops;
    int _wheelSpeed;
    std::string _wheel;

    sftools::Chronometer _shotTimer;
    int _numShots;
    std::stringstream _shotName;

    bool                    _dieing;
    sftools::Chronometer    _dieTimer;
    sftools::Chronometer    _fadeTimer;
    bool                    _fading;
};

class fbSpinShootAction : public jl::Action
{
    public:
        fbSpinShootAction(int loops, ZFunzoBoss * funzoBoss) : Action("SpinShoot", false, loops, funzoBoss) {}

        void update(bool animFinished)
        {
            static_cast<ZFunzoBoss *>(actor())->spinHead("clockwise", 4.0);
            static_cast<ZFunzoBoss *>(actor())->shoot();
            static_cast<ZFunzoBoss *>(actor())->body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));

            Action::update(animFinished);
        }
};

class fbTongueAction : public jl::Action
{
    public:
        fbTongueAction(int loops, ZFunzoBoss * funzoBoss) : Action("TongueWhip", false, loops, funzoBoss) {}

        void update(bool animFinished)
        {
            //static_cast<ZFunzoBoss *>(actor())->tongueWhip();
            static_cast<ZFunzoBoss *>(actor())->body("FunzoHead")->sprite()->setTextureRect(sf::IntRect(0,801,800,800));

            Action::update(animFinished);
        }
};

// --------------------
// FBSHOT
// ---------------------

class FBShot : public jl::Projectile {
public:

    typedef fwk::Ptr<FBShot> Ptr;

    FBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 5.0f);

    FBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 5.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();
};

class fbShotMoveAction : public jl::Action
{
    public:
        fbShotMoveAction(FBShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif




