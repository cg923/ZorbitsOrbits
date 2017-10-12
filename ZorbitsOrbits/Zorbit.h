// Copyright(c) 2017 Happy Rock Studios

#ifndef ZORBIT_H
#define ZORBIT_H

#include <sstream>

#include "JollyLava/Character.h"

#include "ZGrapple.h"

class ZorbitLevel;
class GrappleChain;

class Zorbit : public jl::Character {
public:

    typedef fwk::Ptr<Zorbit> Ptr;

    Zorbit(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, bool vex = false);

    void createBodies(b2Vec2 position, float angle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void reset();

    void resetValues();

    void setPosition(b2Vec2 position);

    void handle(std::string input);

    //Zorbit specific movements
    /**
     * @brief Moves Zorbit at his default max speed.
     * */
    void move(std::string direction);

    void hit(int damage);

    void bounce();

    /// Float "up" relative to Zorbit's rotation in outer space.
    void floatUp();

    /// Float "down" relative to Zorbit's rotation in outer space.
    void floatDown();

    // TODO naming...
    /// Float "right" while Jumping or Bouncing.
    void floatRight();

    void floatLeft();

    void setMaxSpeed(int speed);

    int getMaxSpeed() { return _maxSpeed; }

    void setHealth(int health);

    void shoot();

    void stop();

    void stopCompletely();

    void unStop();

    // Grapple hook related functions
    void createGrappleChain();

    ///Returns the grapple point that is closest to Zorbit, if any are nearby.
    ZGrapple::Ptr closestGrapple();

    ///Removes a grapple point from the grapple list.
    void removeFromGrapList(std::string name);

    void connectToGrapple();

    void disconnectFromGrapple();

    void swingLeft();

    void swingRight();

    void changeGrappleLength(float input);

    bool grappling() const { return _grappling; }

    bool belowGrapple() const;

    std::list<ZGrapple::Ptr> grapPoints() const { return _grapPoints; }

    float bounceStrength() const { return _bounceStrength; }

    int currentHealth() const { return _currentHealth; }

    int currentCogs() const { return _numCogs; }

    void increaseCogs(int increase) { _cogsToIncrease = increase; } // The point of this method is to do it and make it flashy.

    void footContactsMinusMinus() { _footContacts--; }

    void footContactsIs(int value) { _footContacts = value; }

    void printGrapList();

    // Misc.
    void changeToDeathTexture();

    void changeToOriginalTexture();

    void cleanUp();

    void bossUpdates();

    void blink();

    void unblink();

    void invincible() { _invincible = true; }

private:
    friend class ZorbitLevel;

    bool _vex;

    void destroyJoints();
    void die();

    void enableWheel();
    void disableWheel();

    bool _canMove;

    /// The joint connecting Zorbit to his wheel
    b2WheelJoint * _wheelJoint;

    /// Used for certain calculations based on Zorbit's wheel size.
    float _wheelRadius;

    /// The joint connecting Zorbit to his gun on his left
    b2RevoluteJoint * _gunLeftJoint;

    /// The joint connecting Zorbit to his gun on his right
    b2RevoluteJoint * _gunRightJoint;

    /// The joint connecting Zorbit to his grapple point above him
    b2RevoluteJoint * _grapplePointJoint;

    /// The joint connecting  Zorbit to a grapple point
    b2RopeJoint * _grappleJoint;

    /// Currently attached ZGrapple.
    ZGrapple * _currentGrapple;

    /// Keeps track of maximum number of ZShots allowed.
    int MAX_ZSHOTS;

    /// Current number of ZShots in the world.
    int _numZShots;

    /// Current number of Cogs collected.
    int _numCogs;

    /// Cogs to give Zorbit.
    int _cogsToIncrease;

    /// Factor by which Zorbit's speed is increased when he boosts
    int _boostSpeed;

    /// Damage currently being taken by Zorbit, dependent on number of enemies around.
    int _damageTaken;

    float _width;
    float _height;

    /// Timer to restrict ZShots firing rate.
    sftools::Chronometer _shotTimer;

    /// Timer to keep Zorbit briefly invincible after a hit.
    sftools::Chronometer _invincibleTimer;

    /// Timer to coordinate death sequence.
    sftools::Chronometer _deathTimer;

    sftools::Chronometer _junkyardOffScreenTimer;

    /// Used to dynamically name ZShots.
    std::stringstream _shotName;

    /// Used to set direction of ZShots.
    std::string _shotDirection;

    /// List of grapple points near enough to grapple to.
    std::list<ZGrapple::Ptr>_grapPoints;

    /// Whether Zorbit is currently grappling
    bool _grappling;

    /// Whether Zorbit is currently swinging left
    bool _swingLeft;

    /// Whether Zorbit is currently swinging right
    bool _swingRight;

    /// Whether Zorbit is currently moving right while in a gravity field but not on the ground.
    bool _floatRight;

    /// Whether Zorbit is currently moving left while in a gravity field but not on the ground.
    bool _floatLeft;

    /// Maximum distance for a Grapple
    float GRAPPLE_RADIUS;

    /// GrappleChain to visually represent the grapple
    GrappleChain * _grappleChain;

    /// The direction Zorbit should bounce.
    b2Vec2 _bounceVec;

    bool _invincible;

    float _bounceStrength;

    bool _dieing;

    bool _stunned;
    sftools::Chronometer _stunClock;

    sftools::Chronometer _blinkTimer;

    float _speedMultiplier;

    sftools::Chronometer  _spaceTimer;

    bool _resetPrompt;
};

class zStopAction : public jl::Action
{
    public:
        zStopAction(Zorbit * zorbit) : Action("Stop", false, 0, zorbit){}

        zStopAction(float duration, Zorbit * zorbit) : Action("Stop", true, duration, zorbit){}

        void update(bool animFinished)
        {
            static_cast<Zorbit *>(actor())->stop();

            Action::update(animFinished);
        }
};

class zGrappleAction : public jl::Action
{
    public:
        zGrappleAction(Zorbit * zorbit) : Action("Grapple", false, 0, zorbit){}

        zGrappleAction(float duration, Zorbit * zorbit) : Action("Grapple", true, duration, zorbit){}

        void update(bool animFinished)
        {
            Action::update(animFinished);
        }
};

class zBounceAction : public jl::Action
{
    public:
        zBounceAction(Zorbit * zorbit) : Action("Bounce", false, 1, zorbit)
        {
            _bouncing = false;
            _bounceClock.resume();
        }
        void update(bool animFinished)
        {
            if(!_bouncing)
            {
               static_cast<Zorbit *>(actor())->bounce();
               _bouncing = true;
            }

            if(_bouncing && _bounceClock.getElapsedTime().asMilliseconds() > 200)
            {
                actor()->actionMan()->endAction("Bounce");
                reset();
            }

            Action::update(animFinished);
        }
        void reset()
        {
            _bouncing = false;
            Action::reset();
        }

    private:
        bool        _bouncing;
        sftools::Chronometer   _bounceClock;
};

class zFallAction : public jl::Action
{
    public:
        zFallAction(Zorbit * zorbit) : Action("Fall", false, 0, zorbit){}

        zFallAction(float duration, Zorbit * zorbit) : Action("Fall", true, duration, zorbit){}

        void update(bool animFinished)
        {
            Action::update(animFinished);
        }
};

class zDieAction : public jl::Action
{
    public:
        zDieAction(Zorbit * zorbit) : Action("Die", false, 1, zorbit) {}

        void update(bool animFinished)
        {
            if(static_cast<Zorbit*>(actor())->grappling())
                static_cast<Zorbit*>(actor())->disconnectFromGrapple();

            if(animFinished)
                actor()->die();

            Action::update(animFinished);
        }
};


#endif

