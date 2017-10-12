// Copyright(c) 2017 Happy Rock Studios

#ifndef ZMOVINGPLATFORM_H
#define ZMOVINGPLATFORM_H

#include <sstream>

#include "JollyLava/Actor.h"

class ZorbitLevel;

class ZMovingPlatform : public jl::Actor {
public:

    typedef fwk::Ptr<ZMovingPlatform> Ptr;

    ZMovingPlatform(std::string name,
                    ZorbitLevel * level,
                    std::vector<string> layerNames,
                    b2Vec2 initPos,
                    int initAngle,
                    sf::FloatRect rect,
                    bool movingAtStart,
                    std::string direction);

    void readySet();

    void reset();

    void defineFixtures(float width, float height);

    void defineAnimations();

    void defineActions();

    void defineSounds();

    float cameraAngle();

    void beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD);

    void updateConcrete();

    void draw();

    void setDirection(std::string direction) {_direction = direction; }

    void switchDirection();

    void changeMoveSpeed(float factor);

    void stop() {}

    void move(std::string direction);

    bool moving() const { return _moving; }

    std::string direction() const { return _direction; }

private:
    void makeConnections();

    sf::FloatRect _initRect;

    bool        _moving;
    std::string _initDirection;
    std::string _direction;
    float       _velocity;
    float       _initVelocity;

    b2FixtureDef _fixtureDef;
    b2DistanceJoint * _distanceJoint;

    std::map<std::string, Collidable::Ptr> _connections;
    std::map<std::string, Collidable::Ptr> _pendingConnections;
    std::map<std::string, b2Joint*> _joints;

    jl::TextureDesc::Ptr _desc;
    sf::Sprite      _sprite;
};

class pMRightAction : public jl::Action
{
    public:
        pMRightAction(jl::Actor * actor, std::string name = "MoveRight") : jl::Action(name, false, 0, actor){ }
        pMRightAction(float loops, jl::Actor * actor, std::string name = "MoveRight") : jl::Action(name, true, loops, actor){ }

        void setLoops(std::string value)
        {
            _initLoops = atoi(value.c_str());
            _terminates = true;
        }

        void update(bool animFinished)
        {
            static_cast<ZMovingPlatform *>(actor())->move("Right");

            Action::update(animFinished);
        }
};

class pMLeftAction : public jl::Action
{
    public:
        pMLeftAction(jl::Actor * actor, std::string name = "MoveLeft") : jl::Action(name, false, 0, actor){ }
        pMLeftAction(float loops, jl::Actor * actor, std::string name = "MoveLeft") : jl::Action(name, true, loops, actor){ }

        void setLoops(std::string value)
        {
            _initLoops = atoi(value.c_str());
            _terminates = true;
        }

        void update(bool animFinished)
        {
            static_cast<ZMovingPlatform *>(actor())->move("Left");

            Action::update(animFinished);
        }
};

#endif


