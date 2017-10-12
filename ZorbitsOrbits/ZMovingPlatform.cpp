
// Copyright(c) 2017 Happy Rock Studios

#include "ZMovingPlatform.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "JollyLava/Tool.h"
#include "JollyLava/Prop.h"

ZMovingPlatform::ZMovingPlatform(std::string name,
                                 ZorbitLevel * level,
                                 std::vector<string> layerNames,
                                  b2Vec2 initPos,
                                  int initAngle,
                                  sf::FloatRect rect,
                                  bool moving,
                                  std::string direction) : jl::Actor(name,
                                                            level,
                                                            layerNames,
                                                            "MovingPlatform")
{
    _initRect = rect;
    _initRect.height *= 1;
    _velocity = 0.0f;
    _initVelocity = 0.0f;
    _initDirection = direction;
    _direction = direction;
    _moving = false;

    // Filepath for body texture
    std::stringstream ss;
    ss << "levels/" << this->level()->name() << "/images/" << this->name() <<".png";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png",
            sf::Vector2f(1, 1), 1, 1);

    // I wish I knew why the fuck this has to be like this.
    sf::Vector2f rect2 = jl::Tool::sfVec(b2Vec2(_initRect.width, _initRect.height));
    _desc = this->level()->game()->textureMan()->textureDescIs(ss.str(),
            sf::Vector2f(rect2.x / 2, rect2.y / 2), rect2.x, rect2.y);
    _sprite.setTexture(*_desc->texture());
    _sprite.setOrigin(_desc->texture()->getSize().x / 2, _desc->texture()->getSize().y / 2);
    _sprite.setTextureRect(sf::IntRect(0, 0, _desc->texture()->getSize().x, _desc->texture()->getSize().y));

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("ZMovingPlatformBody", "foreground", bodyTex, &bodyDef, initPos, initAngle, b2_kinematicBody);
    bodyNew("ZMovingPlatformSensorBody", "foreground", bodyTex, &bodyDef, initPos, initAngle, b2_dynamicBody);

    defineFixtures(_initRect.width, _initRect.height);
    defineAnimations();
    defineActions();
    defineSounds();
}

void ZMovingPlatform::reset()
{
    this->level()->b2world()->DestroyJoint(_distanceJoint);
    _distanceJoint = NULL;

    Actor::reset();

    if(gravityField())
    {
        b2DistanceJointDef jointDef;
        jointDef.bodyA = rootBody()->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = true;
        jointDef.length = jl::Tool::distance(this->position(), gravityField()->position());
        _distanceJoint = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);
    }

    _direction = _initDirection;
}

void ZMovingPlatform::defineFixtures(float width, float height)
{
    // Main fixture
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width, height, b2Vec2(0,0), 0);
    _fixtureDef.shape = &fixShape;
    _fixtureDef.density = 1.0f;
    _fixtureDef.friction = 3.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Shape");
    rootBody()->fixtureNew(fixUD, &_fixtureDef);

    _fixtureDef.isSensor = true;

    jl::FixtureUserData::Ptr fixUD2 = new jl::FixtureUserData("Root");
    body("ZMovingPlatformSensorBody")->fixtureNew(fixUD2, &_fixtureDef);
}

void ZMovingPlatform::defineAnimations()
{
    _knownAnims["Stop"]         = new jl::Animation("Stop", sf::IntRect(0, 0, _initRect.width, _initRect.height), 1, false);
    _knownAnims["MoveLeft"]     = new jl::Animation("MoveLeft", sf::IntRect(0, 0, _initRect.width, _initRect.height), 1, false);
    _knownAnims["MoveRight"]    = new jl::Animation("MoveRight", sf::IntRect(0, 0, _initRect.width, _initRect.height), 1, false);
    currentAnimIs(_knownAnims["Stop"]);
}

void ZMovingPlatform::defineActions()
{
    _knownActions["MoveRight"]      = new pMRightAction(this);
    _knownActions["MoveLeft"]       = new pMLeftAction(this);
    _knownActions["Stop"]           = new jl::aStopAction(1, this);
}

void ZMovingPlatform::defineSounds()
{
    level()->soundMan()->soundNew("movingplatform", "resources/sounds/movingplatform.wav", false);
}

void ZMovingPlatform::readySet()
{
    Entity::readySet();

    if(gravityField())
    {
        b2DistanceJointDef jointDef;
        jointDef.bodyA = rootBody()->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = true;
        jointDef.length = jl::Tool::distance(this->position(), gravityField()->position());
        _distanceJoint = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);
    }
}

void ZMovingPlatform::makeConnections()
{
    std::map<std::string, Collidable::Ptr>::iterator itr;

    for(itr = _pendingConnections.begin(); itr != _pendingConnections.end(); itr++)
    {
        b2WeldJointDef jd;
        jd.Initialize(rootBody()->b2body(),
                      static_cast<Entity*>(itr->second.ptr())->rootBody()->b2body(),
                      itr->second->position());

        jd.collideConnected = false;
        _joints[itr->second->name()] = (b2WeldJoint *)this->level()->b2world()->CreateJoint(&jd);
        _connections[itr->second->name()] = itr->second;

        static_cast<jl::Prop*>(itr->second.ptr())->anchoredIs(true);
    }

    _pendingConnections.clear();
}

void ZMovingPlatform::updateConcrete()
{
    _sprite.setPosition(jl::Tool::sfVec(rootBody()->b2body()->GetPosition()));
    _sprite.setRotation(-jl::Tool::sfAngle(polarAngle() - jl::Tool::PI() / 2));

    if(!_pendingConnections.empty())
        makeConnections();

    if(gravityField())
    {
        rootBody()->b2body()->SetTransform(position(), polarAngle() - jl::Tool::PI() / 2);
        body("ZMovingPlatformSensorBody")->b2body()->SetTransform(position(), polarAngle() - jl::Tool::PI() / 2);
    }

    if(noActions())
        _actionMan->pushAction(_knownActions["Stop"]);

    if(_moving)
    {
        if(_direction.compare("Right") == 0)
            _actionMan->pushAction(_knownActions["MoveRight"]);
        else
            _actionMan->pushAction(_knownActions["MoveLeft"]);

        level()->soundMan()->playSound("movingplatform", static_cast<ZorbitLevel*>(level())->distanceToZorbit(position()));
    }

    if(!_moving)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
    }
}

void ZMovingPlatform::draw()
{
    level()->game()->renderWindow()->draw(_sprite);
}

void ZMovingPlatform::move(std::string direction)
{
    if(direction == "Right")
    {
        float theta = polarAngle();
        float forceX = _velocity * sin(theta);
        float forceY = -_velocity * cos(theta);
        rootBody()->b2body()->SetLinearVelocity(b2Vec2(forceX, forceY));
        body("ZMovingPlatformSensorBody")->b2body()->SetLinearVelocity(b2Vec2(forceX, forceY));
    }
    else if(direction == "Left")
    {
        float theta = polarAngle();
        float forceX = -_velocity * sin(theta);
        float forceY = _velocity * cos(theta);
        rootBody()->b2body()->SetLinearVelocity(b2Vec2(forceX, forceY));
        body("ZMovingPlatformSensorBody")->b2body()->SetLinearVelocity(b2Vec2(forceX, forceY));
    }
}

void ZMovingPlatform::switchDirection()
{
    if(_direction.compare("Right") == 0)
        _direction = "Left";
    else if(_direction.compare("Left") == 0)
        _direction = "Right";
    else throw fwk::InternalException("Invalid direction!");
}

void ZMovingPlatform::changeMoveSpeed(float factor)
{
    if(factor == 0)
    {
        _initVelocity = _velocity;
        rootBody()->b2body()->SetLinearVelocity(b2Vec2(0, 0));
        body("ZMovingPlatformSensorBody")->b2body()->SetLinearVelocity(b2Vec2(0, 0));
        _moving = false;
    }
    else if (factor == -1)
    {
        _moving = true;
        _velocity = _initVelocity;
    }
    else
    {
        _moving = true;
        _velocity = factor;
    }
}

void ZMovingPlatform::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Grapple") == 0 || thatType.compare("Boinger") == 0)
    {
        if(static_cast<jl::Prop*>(thatCollidable.ptr())->dynamic())
        {
            if(_pendingConnections.count(thatCollidable->name()) == 0)
                _pendingConnections[thatCollidable->name()] = thatCollidable;
        }
    }
}

void ZMovingPlatform::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
}

float ZMovingPlatform::cameraAngle()
{
    float uprightAngle = polarAngle() - 0.5 * jl::Tool::PI();
    return - jl::Tool::sfAngle(uprightAngle);
}


