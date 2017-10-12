// Copyright(c) 2017 Happy Rock Studios

#ifndef ZGLOBBOSS_H
#define ZGLOBBOSS_H

#include <sstream>

// Jollylava
#include "JollyLava/Projectile.h"

// JollyZorbit
#include "ZBoss.h"

class ZorbitLevel;

class ZGlobBoss : public ZBoss {
public:

    typedef fwk::Ptr<ZGlobBoss> Ptr;

    ZGlobBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle);

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineSounds();

    void defineActions();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void readySet();

    void reset();

    void destroyJoints();

    //ZGlobBoss specific movements
    void attack(std::string direction, std::string bodyName);

    void headAttack(std::string direction);

    void tentacleAttack(int which, std::string direction);

    void moveRandom(std::string bodyName);

    void phase1();

    void phase2();

    void phase3();

    void appear(std::string bodyName);

    void disappear(std::string bodyName);

    void flipTentacleSprite(int which);

    std::string direction();

    void makeInvisible(std::string bodyName);

    void makeVisible(std::string bodyName);

    void animate();

    void draw();

    void cleanUp();

private:
    friend class gbDieAction;
    int _numShots;
    int _headPos;

    bool _headVisible;
    bool _tentVisible1;
    bool _tentVisible2;

    bool _tentRight1;
    bool _tentRight2;

    b2DistanceJoint* _tentacleJoint1;
    b2DistanceJoint* _tentacleJoint2;
    b2DistanceJoint* _headJoint;

    bool _tentOverlap1;
    bool _tentOverlap2;

    std::map<std::string, b2Vec2> _positions;

    sf::Sprite _mouthSprite;
    sftools::Chronometer _mouthTimer;
};

class gbMoveAction : public jl::Action
{
   public:
        gbMoveAction(jl::Actor * actor, std::string bodyName) : jl::Action("Move", true, 1, actor), _bodyName(bodyName)
        {
            specifyBody(_bodyName);
        }

        void update(bool animFinished)
        {
            static_cast<ZGlobBoss *>(actor())->moveRandom(_bodyName);

            finish();
        }

    private:
        std::string _bodyName;
};

class gbAttackAction : public jl::Action
{
    public:
        gbAttackAction(jl::Actor * actor,
                       std::string animName,
                       std::string bodyName,
                       std::string direction) :
                           jl::Action(animName, true, 1, actor),
                           _hasAttacked(false),
                           _direction(direction)
        {
            specifyBody(bodyName);
        }

        void reset()
        {
            _hasAttacked = false;
            Action::reset();
        }

        void update(bool animFinished)
        {
            if(!_hasAttacked)
            {
                static_cast<ZGlobBoss *>(actor())->attack(_direction, _bodyName);
                _hasAttacked = true;
            }
            jl::Action::update(animFinished);
        }
    private:
        bool            _hasAttacked;
        std::string     _direction;
};

class gbAppearAction : public jl::Action
{
    public:
        gbAppearAction(jl::Actor * actor, std::string bodyName) : jl::Action("Appear", true, 1, actor), _bodyName(bodyName)
        {
            specifyBody(_bodyName);
        }

        void update(bool animFinished)
        {
            jl::Action::update(animFinished);
            static_cast<ZGlobBoss *>(actor())->appear(_bodyName);
        }

    private:
        std::string _bodyName;
};

class gbDisappearAction : public jl::Action
{
    public:
        gbDisappearAction(jl::Actor * actor, std::string bodyName) : jl::Action("Disappear", true, 1, actor), _bodyName(bodyName)
        {
            specifyBody(_bodyName);
        }

        void update(bool animFinished)
        {
            static_cast<ZGlobBoss *>(actor())->disappear(_bodyName);

            if(animFinished)
                static_cast<ZGlobBoss *>(actor())->makeInvisible(_bodyName);

            jl::Action::update(animFinished);
        }
    private:
        std::string _bodyName;
};

class gbDieAction : public jl::Action
{
    public:
        gbDieAction(jl::Character * character) : jl::Action("Die", false, 1, character), _sound("") { specifyBody("HeadBody");}
        gbDieAction(std::string sound, jl::Character * character) : jl::Action("Die", false, 1, character), _sound(sound), _finished(false) {specifyBody("HeadBody");}

        void update(bool animFinished);

    private:
        std::string _sound;
        bool _finished;
};

class GBShot : public jl::Projectile {
public:

    typedef fwk::Ptr<GBShot> Ptr;

    GBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration = 2.0f);

    GBShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration = 2.0f);

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    float cameraAngle() { return angle(); }

    void defineAnimations();

    void defineSounds() {}

    void defineActions();
};

class gbShotMoveAction : public jl::Action
{
    public:
        gbShotMoveAction(GBShot * shot) : Action("Move", false, 0, shot) {}

        void update(bool animFinished)
        {
            static_cast<jl::Projectile *>(actor())->move();

            Action::update(animFinished);
        }
};

#endif




