// Copyright(c) 2017 Happy Rock Studios

#ifndef ZBOSS_H
#define ZBOSS_H

#include <sstream>

// JollyLava
#include "JollyLava/Character.h"

class ZorbitLevel;

class ZBoss : public jl::Character {
public:

    typedef fwk::Ptr<ZBoss> Ptr;

    void activate();

    void deactivate() {_active = false;}

protected:

    ZBoss(std::string name,
        ZorbitLevel * level,
        std::vector<string> layerNames);

    virtual void defineFixtures(float width, float height) {}

    virtual void defineAnimations() {};

    virtual void defineSounds() {};

    virtual void defineActions() {};

    virtual void reset();

    virtual void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
    {
        jl::Character::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);
    }

    virtual void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
    {
        jl::Character::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
    }

    virtual void handleDie();

    virtual void changeToDeathTexture() {}

    virtual void prepDieConcrete() {}

    virtual void updateConcrete() {}

    void attack() {}

    std::string direction()
    {
        if(directedRight())
            return "Right";
        else return "Left";
    }

    void randomDirection()
    {
        int direction = rand() % 2 + 1;
        if(direction == 1 && !directedRight())
            switchDirection();
        else if(direction == 2 && directedRight())
            switchDirection();

        if(direction != 1
            && direction != 2)
            throw fwk::InternalException("ZBoss: Improper calculation");
    }

    void hit();

    bool _active;

    /// Is Zorbit in sight?
    bool _seesZorbit;

    /// Is Zorbit close enough that he is being attacked?
    bool _attacking;
    jl::Collidable::Ptr _attackTarget;

    int _lFootContacts;
    int _rFootContacts;

protected:
    sftools::Chronometer _deathTimer;

private:
    bool _cameraMoved;
};

class bDieAction : public jl::Action
{
    public:
        bDieAction(jl::Character * character) : jl::Action("Die", false, 1, character), _sound("") {}
        bDieAction(std::string sound, jl::Character * character) : jl::Action("Die", false, 1, character), _sound(sound), _finished(false) {}

        void update(bool animFinished);

    protected:
        std::string _sound;
        bool _finished;
};

#endif



