// Copyright(c) 2017 Happy Rock Studios

#ifndef ZKOPS_H
#define ZKOPS_H

#include <sstream>

#include "JollyLava/Character.h"

class ZorbitLevel;

class ZKops : public jl::Character {
public:

    typedef fwk::Ptr<ZKops> Ptr;

    ZKops(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    //ZKops specific movements
    void attack();

    std::string direction();

    void setMaxSpeed(int maxSpeed) { _maxSpeed = maxSpeed; }

private:
    /// Is Zorbit in sight?
    bool _leftSensor;
    bool _rightSensor;
    bool _attack;

    //int _cogsStolen;

    int _lFootContacts;
    int _rFootContacts;

    sftools::Chronometer _attackTimer;

};

class kAttackAction : public jl::Action
{
    public:
        kAttackAction(jl::Actor * actor) : jl::Action("Attack", false, 0, actor){ }

        void update(bool animFinished)
        {
            if(_attackTimer.getElapsedTime().asMilliseconds() > 300)
            {
                static_cast<ZKops*>(actor())->attack();
                _attackTimer.reset(true);
            }
            jl::Action::update(animFinished);
        }
    private:
        sftools::Chronometer _attackTimer;
};

#endif



