
// Copyright(c) 2017 Happy Rock Studios

#include "ZVexBoss.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "Zorbit.h"
#include "ZShot.h"
#include "JollyLava/Tool.h"

ZVexBoss::ZVexBoss(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle) : ZBoss("VexBoss",
            level, layerNames)
{
    _prefix = "vb";

    // Body texture
    jl::TextureDesc::Ptr bodyTex;
    bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/vexboss/torso.png",
            sf::Vector2f(90, 85), 180, 170);

    // Wheel texture
    jl::TextureDesc::Ptr wheelTex;
    wheelTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/vexboss/wheel.png",
            sf::Vector2f(22, 22), 45, 45);

    // Blank texture, used for gun position
    jl::TextureDesc::Ptr gunTex;
    gunTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png",
            sf::Vector2f(1, 1), 1, 1);

    // Create root body.
    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.0125f;
    bodyDef.angularDamping = 0.0f;
    bodyNew("VexBody", "foreground", bodyTex, &bodyDef, initPos, initAngle);
    rootBodyIs("VexBody");

    // Wheel body
    b2BodyDef wheelBodyDef;
    wheelBodyDef.linearDamping = 0.0f;
    wheelBodyDef.angularDamping = 0.0f;
    float xDistToWheel = 1.2 * -sin(jl::Tool::b2Angle(initAngle));
    float yDistToWheel = 1.1 * -cos(jl::Tool::b2Angle(initAngle));
    bodyNew("VexWheel", "background", wheelTex, &wheelBodyDef, initPos + b2Vec2(xDistToWheel, yDistToWheel), initAngle);

    // Turn off linear damping for all other bodies.
    bodyDef.linearDamping = 0.0f;

    // Gun Right body
    float xDistToGun = 1.25 * cos(jl::Tool::b2Angle(initAngle));
    float yDistToGun = 1.25 * -sin(jl::Tool::b2Angle(initAngle));
    bodyNew("VexGunRight", "foreground", gunTex, &bodyDef, initPos + b2Vec2(xDistToGun, yDistToGun), 0);

    // Gun Left body
    xDistToGun *= -1;
    yDistToGun *= -1;
    bodyNew("VexGunLeft", "foreground", gunTex, &bodyDef, initPos + b2Vec2(xDistToGun, yDistToGun), 0);

    float width = 1.0f;
    float height = 1.4f;

    // Set values
    _active         = false;
    _maxHealth      = 100;
    _currentHealth  = _maxHealth;
    _damage         = 1;
    _maxSpeed       = 40;
    _jumpHeight     = 400;
    _lFootContacts  = 0;
    _rFootContacts  = 0;
    _numShots       = 0;
    _attacking      = false;
    _seesZorbit     = false;
    _vulnerable     = true;
    _shoot          = false;
    _phase2         = false;
    _phase3         = false;
    _phase4         = false;
    _blink          = false;
    _distanceJointOn = false;
    directedRightIs(true);

    defineFixtures(width, height);
    defineAnimations();
    defineSounds();
    defineActions();

    _chronos["shottimer"] = _shotTimer;
    _chronos["actiontimer"] = _actionTimer;
    _chronos["blinktimer"] = _blinkTimer;
    _chronos["shottimer"].resume();
    _chronos["actiontimer"].resume();
    _chronos["blinktimer"].resume();
}

void ZVexBoss::reset()
{
    if(_distanceJointOn)
    {
        level()->b2world()->DestroyJoint(_distanceJoint);
        _distanceJointOn = false;
    }
    _vulnerable = true;
    _shoot = false;
    unblink();
    ZBoss::reset();
}

void ZVexBoss::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width * 0.45, height); // If you change this you have to adjust density.
    fixDef.shape = &fixShape;
    fixDef.density = 2.5f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    body("VexBody")->fixtureNew(fixUD, &fixDef);

    // Wheel fixture definition.
    float wheelRadInPixels = 19.5f;
    _wheelRadius = jl::Tool::pixels2meters(wheelRadInPixels);

    b2CircleShape wheelShape;
    wheelShape.m_radius = _wheelRadius;
    b2FixtureDef wheelFixtureDef;
    wheelFixtureDef.shape = &wheelShape;
    wheelFixtureDef.density = 10.0f;
    wheelFixtureDef.friction = 100.0f; //9.0f;
    wheelFixtureDef.restitution = 0;
    jl::FixtureUserData::Ptr wFixUD = new jl::FixtureUserData(name() + "Wheel");
    body("VexWheel")->fixtureNew(wFixUD, &wheelFixtureDef);

    // Right gun fixture definition
    b2PolygonShape gunShape;
    gunShape.SetAsBox(0.1, 0.1);
    b2FixtureDef gunFixtureDef;
    gunFixtureDef.shape = &gunShape;
    gunFixtureDef.density = 0.0f;
    gunFixtureDef.restitution = 0;
    gunFixtureDef.isSensor = true;

    jl::FixtureUserData::Ptr grFixUD = new jl::FixtureUserData(name() + "GunRight");
    body("VexGunRight")->fixtureNew(grFixUD, &gunFixtureDef);

    // Left Gun
    jl::FixtureUserData::Ptr glFixUD = new jl::FixtureUserData(name() + "GunLeft");
    body("VexGunLeft")->fixtureNew(glFixUD, &gunFixtureDef);

    // Wheel joint.
    b2WheelJointDef jointDef;
    b2Vec2 axis(-0.4f, 1.2f);
    jointDef.Initialize(body("VexBody")->b2body(), body("VexWheel")->b2body(), body("VexWheel")->b2body()->GetPosition(), axis);
    jointDef.motorSpeed = 0.0f;
    jointDef.maxMotorTorque = 100.0f;
    jointDef.enableMotor = false;
    // The next two can be toyed with to get the desired "bounciness".
    jointDef.frequencyHz = 100.0f;       // 15.0f
    jointDef.dampingRatio = 20.0f;       // 8.0f
    _wheelJoint = (b2WheelJoint *)this->level()->b2world()->CreateJoint(&jointDef);

    // Gun right joint.
    b2RevoluteJointDef grJointDef;
    grJointDef.Initialize(body("VexBody")->b2body(), body("VexGunRight")->b2body(), body("VexGunRight")->b2body()->GetPosition());
    _gunRightJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&grJointDef);

    // Gun left joint.
    b2RevoluteJointDef glJointDef;
    glJointDef.Initialize(body("VexBody")->b2body(), body("VexGunLeft")->b2body(), body("VexGunLeft")->b2body()->GetPosition());
    _gunLeftJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&glJointDef);

    jl::Character::defineFixtures(width, height);
}

void ZVexBoss::readySet()
{
    Actor::readySet();
}

void ZVexBoss::defineAnimations()
{
    _knownAnims["Stop"]         = new jl::Animation("Stop",      sf::IntRect(0, 0, 189, 230), 1, true);
    _knownAnims["MoveRight"]    = new jl::Animation("MoveRight", sf::IntRect(0, 0, 189, 230), 4, true, 100);
    _knownAnims["MoveLeft"]     = new jl::Animation("MoveLeft",  sf::IntRect(0, 0, 189, 230), 4, true, 100);
    _knownAnims["HyperSpin"]    = new jl::Animation("HyperSpin", sf::IntRect(756, 0, 189, 230), 1, true);
    _knownAnims["Jump"]         = new jl::Animation("Jump",      sf::IntRect(0, 231, 189, 230), 2, false, 200);
    _knownAnims["Die"]          = new jl::Animation("Die",       sf::IntRect(0, 461, 189, 230), 8, false, 100);

    currentAnimIs(_knownAnims["Stop"]);
}

void ZVexBoss::defineSounds()
{

    level()->soundMan()->soundNew("vb_hit", "resources/sounds/fb_hit.wav", false);
    level()->soundMan()->soundNew("vb_fake_hit", "resources/sounds/fb_fake_hit.wav", false);
    level()->soundMan()->soundNew("vb_shoot", "resources/sounds/vb_shot.wav", false);
    level()->soundMan()->soundNew("vb_power_up", "resources/sounds/vb_power_up.wav", false);
    level()->soundMan()->soundNew("vb_boost", "resources/sounds/vb_boost.wav", false);
    level()->soundMan()->soundNew("vb_die", "resources/sounds/vb_die.wav", false);
}

void ZVexBoss::defineActions()
{
    _knownActions["MoveRight"]  = new jl::cMRightAction(this);
    _knownActions["MoveLeft"]   = new jl::cMLeftAction(this);
    _knownActions["HyperSpin"]  = new vHyperSpinAction(this);
    _knownActions["Jump"]       = new jl::cJumpAction(this, _jumpHeight);
    _knownActions["Stop"]       = new vStopAction(this);
    _knownActions["Die"]        = new bDieAction("fb_die", this);
}

void ZVexBoss::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    ZBoss::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();
    if(thatType.compare("Player") == 0
       && thatFixUD->name().compare("Root") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->hit(1);
    }
    if(thatType.compare("ZShot") == 0
       && thisFixUD->name().compare("Root") == 0
       && _vulnerable)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        decreaseHealth(static_cast<ZShot *>(thatCollidable.ptr())->damage());
        hit();

        if(level()->soundMan()->sound("vb_hit"))
            level()->soundMan()->playSound("vb_hit", true);
    }
    else if(thatType.compare("ZShot") == 0
            && !_vulnerable)
    {
        static_cast<ZShot *>(thatCollidable.ptr())->toDie();
        if(level()->soundMan()->sound("vb_fake_hit"))
            level()->soundMan()->playSound("vb_fake_hit", true);
    }
}

void ZVexBoss::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    ZBoss::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void ZVexBoss::updateConcrete()
{
    if(_currentHealth <= 0
       || static_cast<Zorbit*>(level()->entityMan()->entity("Zorbit").ptr())->currentHealth() <= 0)
    {
        _shoot = false;
        _blink = false;
        _vulnerable = true;
        unblink();
        stopHyperSpin();
    }

    if(!_active)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        _chronos["actiontimer"].reset(true);
    }
    else if (gravityField() && _currentHealth > 0)
    {
        if(noActions())
            _actionMan->pushAction(_knownActions["Stop"]);

        if(_shoot) shoot();

        if(_blink) blink();
        else unblink();

        // Boss logic.
        if(_currentHealth > _maxHealth * .75)
            phase1();
        else if(_currentHealth > _maxHealth / 2)
            phase2();
        else if(_currentHealth > _maxHealth / 4)
            phase3();
        else
            phase4();
    }
}

void ZVexBoss::phase1()
{
    _maxSpeed = 100;

    // PAUSE
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 1
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    // ROLL AND SHOOT
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 1
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 4
        && _actionMan->peekAction()->name().compare("MoveRight") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["MoveRight"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 4.5
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    // JUMP
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4.5
    &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 5.5
    && _actionMan->peekAction()->name().compare("Jump") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Jump"]);
        _shoot = true;
    }
    // OTHER WAY
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.5
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 8.5
        && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["MoveLeft"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8.5
    &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 9.5
    && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    // JUMP
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 9.5
    &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 10
    && _actionMan->peekAction()->name().compare("Jump") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Jump"]);
        _shoot = true;
    }
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 10.5)
    {
        _chronos["actiontimer"].reset(true);
    }
}

void ZVexBoss::phase2()
{
    if(!_phase2)
    {
        _chronos["actiontimer"].reset(true);
        _phase2 = true;
        _blink = true;
    }

    // CHARGE
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 2
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        level()->soundMan()->playSound("vb_power_up");
        _blink = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _vulnerable = false;
        _shoot = false;
    }
    // HYPERSPIN
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 2
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 4
        && _actionMan->peekAction()->name().compare("HyperSpin") != 0)
    {
        level()->soundMan()->playSound("vb_boost");
        _blink = false;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["HyperSpin"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 5.5
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        stopHyperSpin();
        _vulnerable = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    // ROLL
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 5.5
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 7.5
        && _actionMan->peekAction()->name().compare("MoveRight") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["MoveRight"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 7.5
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 8
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = true;
    }
    // JUMP
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8
    &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 9
    && _actionMan->peekAction()->name().compare("Jump") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Jump"]);
        _shoot = true;
    }
    // OTHER WAY
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 9
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 11
        && _actionMan->peekAction()->name().compare("MoveLeft") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["MoveLeft"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 11
    &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 12
    && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = true;
    }
    // JUMP
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 12
    &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 13
    && _actionMan->peekAction()->name().compare("Jump") != 0)
    {
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Jump"]);
        _shoot = true;
    }
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 13)
        _chronos["actiontimer"].reset(true);
}

void ZVexBoss::phase3()
{
    if(!_phase3)
    {
        _chronos["actiontimer"].reset(true);
        _phase3 = true;
    }

    // CHARGE
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 2)
    {
        level()->soundMan()->playSound("vb_power_up");
        _blink = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _vulnerable = false;
        _shoot = false;
    }
    // HYPERSPIN
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 2
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 4
        && _actionMan->peekAction()->name().compare("HyperSpin") != 0)
    {
        level()->soundMan()->playSound("vb_boost");
        _blink = false;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["HyperSpin"]);
        _shoot = false;
    }
    // CHARGE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 6
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        level()->soundMan()->playSound("vb_power_up");
        stopHyperSpin();
        switchDirection();
        _blink = true;
        _vulnerable = false;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    // HYPERSPIN
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 8
        && _actionMan->peekAction()->name().compare("HyperSpin") != 0)
    {
        level()->soundMan()->playSound("vb_boost");
        _blink = false;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["HyperSpin"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 11
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        stopHyperSpin();
        _vulnerable = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 11)
        _chronos["actiontimer"].reset(true);
}

void ZVexBoss::phase4()
{
    if(!_phase4)
    {
        _chronos["actiontimer"].reset(true);
        _phase4 = true;
    }
    // CHARGE
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() < 2)
    {
        level()->soundMan()->playSound("vb_power_up");
        _blink = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _vulnerable = false;
        _shoot = false;
    }
    // HYPERSPIN
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 2
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 4
        && _actionMan->peekAction()->name().compare("HyperSpin") != 0)
    {
        level()->soundMan()->playSound("vb_boost");
        _blink = false;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["HyperSpin"]);
        _shoot = false;
    }
    // CHARGE
    if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 4
       && _chronos["actiontimer"].getElapsedTime().asSeconds() < 6)
    {
        level()->soundMan()->playSound("vb_power_up");
        stopHyperSpin();
        _blink = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _vulnerable = false;
        _shoot = false;
    }
    // JUMP
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 6.1
        && _actionMan->peekAction()->name().compare("Jump") != 0)
    {
        _blink = false;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Jump"]);
    }
    // HYPERSPIN
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 6.1
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 8.1
        && _actionMan->peekAction()->name().compare("HyperSpin") != 0)
    {
        level()->soundMan()->playSound("vb_boost");
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["HyperSpin"]);
        _shoot = false;
    }
    // PAUSE
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 8.1
        &&_chronos["actiontimer"].getElapsedTime().asSeconds() < 11.1
        && _actionMan->peekAction()->name().compare("Stop") != 0)
    {
        stopHyperSpin();
        _blink = false;
        _vulnerable = true;
        _actionMan->clearActions();
        _actionMan->pushAction(_knownActions["Stop"]);
        _shoot = false;
    }
    else if(_chronos["actiontimer"].getElapsedTime().asSeconds() >= 11.1)
    {
        switchDirection();
        _chronos["actiontimer"].reset(true);
    }
}

void ZVexBoss::stop()
{
    _wheelJoint->SetMotorSpeed(0);
    disableWheel();
}

void ZVexBoss::move(std::string direction)
{
    if (_maxSpeed < 0)
        throw fwk::InternalException("Speed must be non-negative.");

    if(direction.compare("Right") == 0)
    {
        if(grounded() && gravityField())
        {
            enableWheel();
            if(polarVelocity().x < _maxSpeed)
            {
                _wheelJoint->SetMotorSpeed(-_maxSpeed / _wheelRadius);
                rootBody()->applyPolarForceToCenter(0,
                                                _maxSpeed * 2 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
            }
        }
        if(!directedRight())
            switchDirection();
    }
    else if(direction.compare("Left") == 0)
    {
        if(grounded() && gravityField())
        {
            enableWheel();
            if(polarVelocity().x > -_maxSpeed)
            {
                _wheelJoint->SetMotorSpeed(_maxSpeed / _wheelRadius);
                rootBody()->applyPolarForceToCenter(0,
                                                -_maxSpeed * 2 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
            }
        }
        if(directedRight())
            switchDirection();
    }
    else
    {
        std::cout << direction << std::endl;
        throw fwk::InternalException("Invalid direction of movement.");
    }
}

void ZVexBoss::enableWheel()
{
    _wheelJoint->EnableMotor(true);
}

void ZVexBoss::disableWheel()
{
    _wheelJoint->EnableMotor(false);
}

void ZVexBoss::blink()
{
    if(_chronos["blinktimer"].getElapsedTime().asMilliseconds() < 100)
    {
        std::map<std::string, jl::Body::Ptr>::iterator itr;
        for(itr = bodyItrBegin(); itr != bodyItrEnd(); itr++)
        {
            itr->second->sprite()->setColor(sf::Color::Blue);
        }
    }
    else if(_chronos["blinktimer"].getElapsedTime().asMilliseconds() < 200)
    {
        std::map<std::string, jl::Body::Ptr>::iterator itr;
        for(itr = bodyItrBegin(); itr != bodyItrEnd(); itr++)
        {
            itr->second->sprite()->setColor(sf::Color(255,255,255,255));
        }
    }
    else
        _chronos["blinktimer"].reset(true);
}

void ZVexBoss::unblink()
{
    std::map<std::string, jl::Body::Ptr>::iterator itr;
    for(itr = bodyItrBegin(); itr != bodyItrEnd(); itr++)
    {
        itr->second->sprite()->setColor(sf::Color(255,255,255,255));
    }
}

void ZVexBoss::shoot()
{
    if(_chronos["shottimer"].getElapsedTime().asMilliseconds() > 500)
    {
        //if(!level()->entityMan()->entity("zVBShot1"))
         //   _numShots = 1;

        // generate shot name
        _shotName.str("");
        _shotName.clear();
        _shotName << "zVBShot" << _numShots;

        // set shot direction
        b2Vec2 gunPos;
        if(directedRight())
        {
            _shotDirection = "Right";
            gunPos = body("VexGunRight")->b2body()->GetPosition();
        }
        else
        {
            _shotDirection = "Left";
            gunPos = body("VexGunLeft")->b2body()->GetPosition();
        }

        float shotRadius = jl::Tool::distance(gunPos, gravityField()->position());
        VShot::Ptr shot = new VShot(_shotName.str(), static_cast<ZorbitLevel *>(level()), _layerNames, gunPos, shotRadius, _shotDirection);
        level()->entityMan()->entityNew(shot->name(), "Layer1", shot);

        _numShots++;
        level()->soundMan()->playSound("vb_shoot", true);
        _chronos["shottimer"].reset(true);
    }
}

void ZVexBoss::hyperSpin()
{
    if(!_distanceJointOn)
    {
        b2DistanceJointDef jointDef;
        jointDef.bodyA = rootBody()->b2body();
        jointDef.bodyB = gravityField()->b2body();
        jointDef.collideConnected = true;
        jointDef.length = jl::Tool::distance(this->position(), gravityField()->position());
        _distanceJoint = (b2DistanceJoint *)this->level()->b2world()->CreateJoint(&jointDef);
        _distanceJointOn = true;
    }
    else
    {
        if(directedRight())
            rootBody()->applyPolarForceToCenter(0, _maxSpeed * 100 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
        else
            rootBody()->applyPolarForceToCenter(0, -_maxSpeed * 100 * gravityField()->specificForce() / gravityField()->defaultSpecificForce());

    }
}

void ZVexBoss::stopHyperSpin()
{
    rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
    if(_distanceJointOn)
    {
        level()->b2world()->DestroyJoint(_distanceJoint);
        _distanceJointOn = false;
    }
}

void ZVexBoss::destroyJoints()
{
    level()->b2world()->DestroyJoint( _wheelJoint );
    level()->b2world()->DestroyJoint( _gunLeftJoint );
    level()->b2world()->DestroyJoint( _gunRightJoint );
    if(_distanceJointOn)
        level()->b2world()->DestroyJoint(_distanceJoint);
}

// VSHOT METHODS
VShot::VShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float distance, std::string direction, float duration) : jl::Projectile(name,
            level, layerNames, direction, duration, "VShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/vshot/vshot.png",
            sf::Vector2f(25, 25), 50, 50);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("VShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = distance;
    _spaceAngle = 0;

    // Set values
    _maxSpeed       = 30;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

    gravityFieldIs(this->level()->closestGravityField(rootBody()->b2body()->GetPosition()));

}

VShot::VShot(std::string name, ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, float angle, float duration) : jl::Projectile(name,
            level, layerNames, "Space", duration, "VShot")
{
    jl::TextureDesc::Ptr tex;
    tex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/vshot/vshot.png",
            sf::Vector2f(10, 10), 20, 20);

    // Create root body.
    b2BodyDef bodyDef;
    bodyNew("VShotBody", "foreground", tex, &bodyDef, initPos, 0);
    rootBody()->b2body()->SetLinearDamping(0);

    _initialDistance = 0;
    _spaceAngle = angle;

    // Set values
    _maxSpeed       = 30;
    _damage         = 1;

    float width     = 0.25;
    float height    = 0.25;

    defineFixtures(width, height);
    defineAnimations();
    defineActions();
    defineSounds();

    gravityFieldIs(this->level()->closestGravityField(rootBody()->b2body()->GetPosition()));

}

void VShot::defineAnimations()
{
    _knownAnims["Move"] = new jl::Animation("Move", sf::IntRect(0, 0, 50, 50), 1, true);

    currentAnimIs(_knownAnims["Move"]);
}

void VShot::defineActions()
{
    _knownActions["Move"] = new vbShotMoveAction(this);
}

void VShot::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Player") == 0
       && thisFixUD->name().compare("Root") == 0
       && thatFixUD->name().compare("Root") == 0)
    {
        static_cast<Zorbit *>(thatCollidable.ptr())->decreaseHealth(_damage);
        toDie();

        std::stringstream sfx;
        sfx << static_cast<jl::Character *>(thatCollidable.ptr())->prefix() << "_hit";

        if(level()->soundMan()->sound(sfx.str()))
            level()->soundMan()->playSound(sfx.str(), true);
    }
}

void VShot::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Projectile::endContact(contact, thisFixUD, thatCollidable, thatFixUD);
}

void VShot::updateConcrete()
{
    if(_deathClock.getElapsedTime().asSeconds() >= _duration - 0.3)
        toDie();

    if(noActions())
        _actionMan->pushAction(_knownActions["Move"]);
}

