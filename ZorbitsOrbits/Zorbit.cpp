// Copyright(c) 2017 Happy Rock Studios

#include "Zorbit.h"
#include "ZorbitsOrbits.h"
#include "ZorbitLevel.h"
#include "ZGrappleChain.h"
#include "ZMovingPlatform.h"
#include "ZBoinger.h"
#include "ZBooster.h"
#include "ZCircle.h"
#include "ZShot.h"
#include "ZPlatform.h"
#include "ZElvisBoss.h"
#include "JollyLava/Tool.h"

Zorbit::Zorbit(ZorbitLevel * level, std::vector<string> layerNames, b2Vec2 initPos, int initAngle, bool vex) : jl::Character("Zorbit",
            level, layerNames, "Player")
{
    _prefix = "zorbit";

    _vex = vex;

    _speedMultiplier = 1;
    if(static_cast<ZorbitsOrbits*>(this->level()->game())->_fastMode) _speedMultiplier = 2;
    _maxSpeed = 12 * _speedMultiplier;

    _maxHealth      = 10;
    _currentHealth  = 10;
    if(this->level()->game()->difficulty().compare("Insane") == 0)
    {
        _maxHealth = 1;
        _currentHealth = 1;
    }
    _jumpHeight     = 60;
    _boostSpeed     = 20;
    _numCogs        = 0;
    _cogsToIncrease = 0;
    _numZShots      = 1;
    _damageTaken    = 0;
    _shotDirection  = "Right";

    _grappling      = false;
    _swingLeft      = false;
    _swingRight     = false;
    _floatLeft      = false;
    _floatRight     = false;
    _dieing         = false;
    _stunned        = false;
    _invincible     = false;
    _canMove        = true;

    directedRightIs(true);

    MAX_ZSHOTS      = 50;
    GRAPPLE_RADIUS  = 5;

    _bounceVec = b2Vec2(0,0);
    _bounceStrength = 0;

    createBodies(initPos, initAngle);
    _width = 0.7f;
    _height = 1.0f;
    defineFixtures(_width, _height);
    defineAnimations();
    defineActions();
    defineSounds();
    createGrappleChain();

    _chronos["shot"] = _shotTimer;
    _chronos["invincible"] = _invincibleTimer;
    _chronos["death"] = _deathTimer;
    _chronos["junkyard"] = _junkyardOffScreenTimer;
    _chronos["stun"] = _stunClock;
    _chronos["blink"] = _blinkTimer;
    _chronos["space"] = _spaceTimer;
}

void Zorbit::createBodies(b2Vec2 pos, float angle)
{
    jl::TextureDesc::Ptr bodyTex;
    jl::TextureDesc::Ptr wheelTex;

    if(!_vex)
    {
        // Body texture
        bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/zorbit/torso.png",
                sf::Vector2f(62, 66), 150, 150);

        // Wheel texture
        wheelTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/zorbit/wheel.png",
                sf::Vector2f(25, 25), 50, 50);

        // Death texture
        this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/zorbit/zorbitdeath.png",
                sf::Vector2f(175, 125), 350, 250);
    }
    else
    {
        // Body texture
        bodyTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/vexboss/torso.png",
                sf::Vector2f(90, 85), 180, 170);

        // Wheel texture
        wheelTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/vexboss/wheel.png",
                sf::Vector2f(22, 22), 45, 45);

    }

    // Blank texture, used for gun position
    jl::TextureDesc::Ptr gunTex;
    gunTex = this->level()->game()->textureMan()->textureDescIs("resources/zorbitsorbits/blank.png",
            sf::Vector2f(1, 1), 1, 1);

    // Create root body.
    b2BodyDef bodyDef;
    bodyDef.linearDamping = 0.0125f;
    bodyDef.angularDamping = 0.0f;
    bodyNew("ZorbitBody", "foreground", bodyTex, &bodyDef, pos, angle);
    rootBodyIs("ZorbitBody");

    // Wheel body
    b2BodyDef wheelBodyDef;
    wheelBodyDef.linearDamping = 0.0f;
    wheelBodyDef.angularDamping = 0.0f;
    float xDistToWheel = -sin(jl::Tool::b2Angle(angle));
    float yDistToWheel = 1.1 * -cos(jl::Tool::b2Angle(angle));
    bodyNew("ZorbitWheel", "background", wheelTex, &wheelBodyDef, pos + b2Vec2(xDistToWheel, yDistToWheel), angle);

    // Turn off linear damping for all other bodies.
    bodyDef.linearDamping = 0.0f;

    // Gun Right body
    float xDistToGun = 1.25 * cos(jl::Tool::b2Angle(angle));
    float yDistToGun = 1.25 * -sin(jl::Tool::b2Angle(angle));
    bodyNew("ZorbitGunRight", "foreground", gunTex, &bodyDef, pos + b2Vec2(xDistToGun, yDistToGun), 0);

    // Gun Left body
    xDistToGun *= -1;
    yDistToGun *= -1;
    bodyNew("ZorbitGunLeft", "foreground", gunTex, &bodyDef, pos + b2Vec2(xDistToGun, yDistToGun), 0);

    // Grapple body
    float xDistToGrap = -sin(jl::Tool::b2Angle(angle));
    float yDistToGrap = cos(jl::Tool::b2Angle(angle));
    bodyNew("ZorbitGrapple", "foreground", gunTex, &bodyDef, pos + b2Vec2(xDistToGrap, yDistToGrap), 0);
}

void Zorbit::defineFixtures(float width, float height)
{
    // Torso fixture
    b2FixtureDef fixDef;
    b2PolygonShape fixShape;
    fixShape.SetAsBox(width * 0.45, height); // If you change this you have to adjust density.
    fixDef.shape = &fixShape;
    fixDef.density = 1.0f;
    fixDef.friction = 0.0f;

    jl::FixtureUserData::Ptr fixUD = new jl::FixtureUserData("Root");
    body("ZorbitBody")->fixtureNew(fixUD, &fixDef);

    // Wheel fixture definition.
    float wheelRadInPixels = 17.5f;
    _wheelRadius = jl::Tool::pixels2meters(wheelRadInPixels);

    b2CircleShape wheelShape;
    wheelShape.m_radius = _wheelRadius;
    b2FixtureDef wheelFixtureDef;
    wheelFixtureDef.shape = &wheelShape;
    wheelFixtureDef.density = 1.0f;
    wheelFixtureDef.friction = 100.0f; //9.0f;
    wheelFixtureDef.restitution = 0;
    jl::FixtureUserData::Ptr wFixUD = new jl::FixtureUserData(name() + "Wheel");
    body("ZorbitWheel")->fixtureNew(wFixUD, &wheelFixtureDef);

    // Right gun fixture definition
    b2PolygonShape gunShape;
    gunShape.SetAsBox(0.1, 0.1);
    b2FixtureDef gunFixtureDef;
    gunFixtureDef.shape = &gunShape;
    gunFixtureDef.density = 0.0f;
    gunFixtureDef.restitution = 0;
    gunFixtureDef.isSensor = true;

    jl::FixtureUserData::Ptr grFixUD = new jl::FixtureUserData("GunRight");
    body("ZorbitGunRight")->fixtureNew(grFixUD, &gunFixtureDef);

    // Left Gun
    jl::FixtureUserData::Ptr glFixUD = new jl::FixtureUserData("GunLeft");
    body("ZorbitGunLeft")->fixtureNew(glFixUD, &gunFixtureDef);

    // Grapple point
    jl::FixtureUserData::Ptr grapFixUD = new jl::FixtureUserData(name() + "Grapple");
    body("ZorbitGrapple")->fixtureNew(grapFixUD, &gunFixtureDef);

    // Grapple zone fixture definition
    b2CircleShape grappleShape;
    grappleShape.m_radius = GRAPPLE_RADIUS;
    b2FixtureDef grapFixtureDef;
    grapFixtureDef.shape = &grappleShape;
    grapFixtureDef.density = 0.0f;
    grapFixtureDef.restitution = 0;
    grapFixtureDef.isSensor = true;

    jl::FixtureUserData::Ptr grapZoneFixUD = new jl::FixtureUserData(name() + "GrappleZone");
    body("ZorbitBody")->fixtureNew(grapZoneFixUD, &grapFixtureDef);

    // Wheel joint.
    b2WheelJointDef jointDef;
    b2Vec2 axis(0.0f, 1.0f);
    jointDef.Initialize(body("ZorbitBody")->b2body(), body("ZorbitWheel")->b2body(), body("ZorbitWheel")->b2body()->GetPosition(), axis);
    jointDef.motorSpeed = 0.0f;
    jointDef.maxMotorTorque = 20.0f;
    jointDef.enableMotor = false;
    // The next two can be toyed with to get the desired "bounciness".
    jointDef.frequencyHz = 15.0f;       // 15.0f
    jointDef.dampingRatio = 8.0f;       // 8.0f
    _wheelJoint = (b2WheelJoint *)this->level()->b2world()->CreateJoint(&jointDef);

    // Gun right joint.
    b2RevoluteJointDef grJointDef;
    grJointDef.Initialize(body("ZorbitBody")->b2body(), body("ZorbitGunRight")->b2body(), body("ZorbitGunRight")->b2body()->GetPosition());
    _gunRightJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&grJointDef);

    // Gun left joint.
    b2RevoluteJointDef glJointDef;
    glJointDef.Initialize(body("ZorbitBody")->b2body(), body("ZorbitGunLeft")->b2body(), body("ZorbitGunLeft")->b2body()->GetPosition());
    _gunLeftJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&glJointDef);

    // Grapple point joint.
    b2RevoluteJointDef grapJointDef;
    grapJointDef.Initialize(body("ZorbitBody")->b2body(), body("ZorbitGrapple")->b2body(), body("ZorbitGrapple")->b2body()->GetPosition());
    _grapplePointJoint = (b2RevoluteJoint *)this->level()->b2world()->CreateJoint(&grapJointDef);

    Character::defineFixtures(width, height);
}

void Zorbit::defineAnimations()
{
    if(!_vex)
    {
        _knownAnims["Stop"]         = new jl::Animation("Stop",      sf::IntRect(0, 10, 150, 150), 1, true);
        _knownAnims["MoveRight"]    = new jl::Animation("MoveRight", sf::IntRect(0, 160, 150, 150), 18, true);
        _knownAnims["MoveLeft"]     = new jl::Animation("MoveLeft",  sf::IntRect(0, 160, 150, 150), 18, true);
        _knownAnims["Jump"]         = new jl::Animation("Jump",      sf::IntRect(150, 10, 150, 150), 2, false, 300);
        _knownAnims["Bounce"]       = new jl::Animation("Bounce",    sf::IntRect(150, 10, 150, 150), 2, false, 300);
        _knownAnims["Grapple"]      = new jl::Animation("Grapple",   sf::IntRect(450, 0, 150, 160), 1, false);
        _knownAnims["Fall"]         = new jl::Animation("Fall",      sf::IntRect(300, 10, 150, 150), 1, false);
        _knownAnims["Hit"]          = new jl::Animation("Hit",       sf::IntRect(600, 10, 150, 150), 1, false);
        _knownAnims["Die"]          = new jl::Animation("Die",       sf::IntRect(0, 0, 350, 250), 10, false, 60);
    }
    else
    {
        _knownAnims["Stop"]         = new jl::Animation("Stop",      sf::IntRect(0, 0, 189, 230), 1, true);
        _knownAnims["MoveRight"]    = new jl::Animation("MoveRight", sf::IntRect(0, 0, 189, 230), 4, true, 100);
        _knownAnims["MoveLeft"]     = new jl::Animation("MoveLeft",  sf::IntRect(0, 0, 189, 230), 4, true, 100);
        _knownAnims["Jump"]         = new jl::Animation("Jump",      sf::IntRect(0, 231, 189, 230), 2, false, 200);
        _knownAnims["Bounce"]       = new jl::Animation("Bounce",    sf::IntRect(0, 231, 189, 230), 2, false, 200);
        _knownAnims["Grapple"]      = new jl::Animation("Grapple",   sf::IntRect(379, 231, 189, 230), 1, false);
        _knownAnims["Fall"]         = new jl::Animation("Fall",      sf::IntRect(190, 231, 189, 230), 1, false);
        _knownAnims["Hit"]          = new jl::Animation("Hit",       sf::IntRect(190, 461, 189, 230), 1, false);
        _knownAnims["Die"]          = new jl::Animation("Die",       sf::IntRect(0, 461, 189, 230), 8, false, 80);
    }

    currentAnimIs(_knownAnims["Stop"]);
}

void Zorbit::defineSounds()
{
    level()->soundMan()->soundNew("zorbit_die", "resources/sounds/zorbit_die.wav", false);
    level()->soundMan()->soundNew("zorbit_hit", "resources/sounds/zorbit_hit.wav", false);
    level()->soundMan()->soundNew("zorbit_jump", "resources/sounds/zorbit_jump.wav", false);
    level()->soundMan()->soundNew("zorbit_land", "resources/sounds/zorbit_land.wav", false);
    level()->soundMan()->soundNew("zorbit_cog_pickup", "resources/sounds/zorbit_cog_pickup.wav", false);
    level()->soundMan()->soundNew("zorbit_shoot", "resources/sounds/zorbit_shoot.wav", false);
    level()->soundMan()->soundNew("zorbit_bounce", "resources/sounds/zorbit_bounce.wav", false);
    level()->soundMan()->soundNew("zorbit_boost", "resources/sounds/zorbit_boost.wav", false);
    level()->soundMan()->soundNew("grap_avail", "resources/sounds/grap_avail.wav", false);
    level()->soundMan()->soundNew("grap_connect", "resources/sounds/grap_connect.wav", false);
}

void Zorbit::defineActions()
{
    _knownActions["MoveRight"]  = new jl::cMRightAction(this);
    _knownActions["MoveLeft"]   = new jl::cMLeftAction(this);
    _knownActions["Jump"]       = new jl::cJumpAction(this, _jumpHeight);
    _knownActions["Bounce"]     = new zBounceAction(this);
    _knownActions["Grapple"]    = new zGrappleAction(this);
    _knownActions["Fall"]       = new zFallAction(this);
    _knownActions["Stop"]       = new zStopAction(this);
    _knownActions["Die"]        = new zDieAction(this);
}

void Zorbit::reset()
{
    resetValues();
    changeToOriginalTexture();

    if(!directedRight())
        switchDirection();

    _nearbyGravityFields.clear();
    _grapPoints.clear();

    setPosition(static_cast<ZorbitLevel*>(level())->checkPointPosition());

    // Get closest gravity field to make sure the entity doesn't start off stuck floating in space.
    gravityFieldIs(level()->closestGravityField(rootBody()->b2body()->GetPosition()));

    // Angle toward it.
    updatePolarAngle();

    _actionMan->clearActions();

}

void Zorbit::resetValues()
{
    _currentHealth  = _maxHealth;
    //_numZShots      = 1;
    _damageTaken    = 0;
    _cogsToIncrease = 0;
    _footContacts   = 0;
    _shotDirection  = "Right";

    _grappling      = false;
    _swingLeft      = false;
    _swingRight     = false;
    _floatLeft      = false;
    _floatRight     = false;
    _stunned        = false;
    _invincible     = false;
    level()->resumeInput();
}

void Zorbit::destroyJoints()
{
    level()->b2world()->DestroyJoint( _wheelJoint );
    level()->b2world()->DestroyJoint( _gunLeftJoint );
    level()->b2world()->DestroyJoint( _gunRightJoint );
    level()->b2world()->DestroyJoint( _grapplePointJoint );
}

void Zorbit::beginContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    if(level()->name().compare("jungle") == 0
       && (thatCollidable->name().compare("Gravity3") == 0
       || thatCollidable->name().compare("Gravity4") == 0))
       return;

    // First run through all collisions handled by superclasses.
    Character::beginContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    // Things Zorbit lands on.
    if(thatType.compare("Planet") == 0 && thisFixUD->name().compare("Bottom") == 0)
    {
        static_cast<ZorbitLevel*>(level())->firstTouchIs(true);
        if(_footContacts == 1 && !static_cast<jl::Planet*>(thatCollidable.ptr())->isSensor())
            level()->soundMan()->playSound("zorbit_land");
    }

    if((thatType.compare("MovingPlatform") == 0 || thatType.compare("RectShape") == 0)
       && thisFixUD->name().compare("Bottom") == 0)
    {
        _footContacts++;
        if(_footContacts == 1)
            level()->soundMan()->playSound("zorbit_land");
    }
    if(thatType.compare("Platform") == 0 && thisFixUD->name().compare("Bottom") == 0)
    {
        if(static_cast<ZPlatform *>(thatCollidable.ptr())->activated())
        {
            _footContacts++;
            if(_footContacts == 1 )
                level()->soundMan()->playSound("zorbit_land");
        }
    }
    if(thatType.compare("Enemy") == 0
       && thisFixUD->name().compare("Bottom") == 0
       && thatFixUD->name().compare("Head") == 0)
    {
        _footContacts++;
        if(_footContacts == 1 )
            level()->soundMan()->playSound("zorbit_land");

        if(static_cast<jl::Character*>(thatCollidable.ptr())->currentHealth() > 0)
        {
            _damageTaken += static_cast<jl::Character*>(thatCollidable.ptr())->damage();
        }
    }

    if(thatType.compare("GravityField") == 0 && thisFixUD->name().compare("Bottom") == 0)
    {
        static_cast<ZorbitLevel*>(level())->bullockedIs(false);
        _jumpHeight = 60 + static_cast<jl::GravityField*>(thatCollidable.ptr())->specificForce() - 30;
    }

    if(thatType.compare("Grapple") == 0 && thisFixUD->name().compare("ZorbitGrappleZone") == 0)
    {
        static_cast<ZGrapple *>(thatCollidable.ptr())->activate();
        _grapPoints.push_back(static_cast<ZGrapple*>(thatCollidable.ptr()));
    }
    if(thatType.compare("Boinger") == 0
       && (thisFixUD->name().compare("Top") == 0
           || thisFixUD->name().compare("Bottom") == 0)
       && thatFixUD->name().compare("Top") == 0)
    {
        if(actionMan()->peekAction()->name() != "Bounce"
           //&& !grounded()
           && static_cast<jl::Prop*>(thatCollidable.ptr())->anchored())
        {
            if(polarVelocity().y < 5 || grappling())
            {
                float angle = thatCollidable->angle();
                _bounceVec = b2Vec2(-sin(angle), cos(angle));
                _bounceStrength = 1.2f * static_cast<ZBoinger*>(thatCollidable.ptr())->strength() + (abs(polarVelocity().y) * 3);

                actionMan()->pushAction(knownAction("Bounce"));
                level()->soundMan()->playSound("zorbit_bounce", true);
            }
        }
    }
    if(thatType.compare("Booster") == 0 && thisFixUD->name().compare("Bottom") == 0)
    {
        float smallerX = std::min(fabs(_boostSpeed * rootBody()->b2body()->GetLinearVelocity().x),
                                  float(static_cast<ZBooster *>(thatCollidable.ptr())->boostStrength()));
        float smallerY = std::min(fabs(_boostSpeed * rootBody()->b2body()->GetLinearVelocity().y),
                                  float(static_cast<ZBooster *>(thatCollidable.ptr())->boostStrength()));

        float desiredX = std::max(smallerX, 0.f);
        float desiredY = std::max(smallerY, 0.f);

        if(rootBody()->b2body()->GetLinearVelocity().x < 0)
            desiredX *= -1;
        if(rootBody()->b2body()->GetLinearVelocity().y < 0)
            desiredY *= -1;

        rootBody()->b2body()->ApplyLinearImpulse(b2Vec2(desiredX, desiredY),
                                                 rootBody()->b2body()->GetWorldCenter());
        level()->soundMan()->playSound("zorbit_boost", true);
    }
    if(thatType.compare("Cog") == 0 && thisFixUD->name().compare("Root") == 0)
    {
        _numCogs++;
        static_cast<jl::Entity*>(thatCollidable.ptr())->die();
        level()->soundMan()->playSound("zorbit_cog_pickup", true);
    }
    if((thatType.compare("Enemy") == 0)
       && (thatFixUD->name().compare("Left") == 0 || thatFixUD->name().compare("Right") == 0)
       && thisFixUD->name().compare("Root") == 0)
    {
        // Enemy must not be dead already
        if(static_cast<jl::Character*>(thatCollidable.ptr())->currentHealth() > 0)
        {
            _damageTaken += static_cast<jl::Character*>(thatCollidable.ptr())->damage();
        }
    }
    if((thatCollidable->name().compare("CircleShape16") == 0)
       && level()->name().compare("jungle") == 0
       && thisFixUD->name().compare("Root") == 0)
    {
        _currentHealth = 0;
    }
}

void Zorbit::endContact(b2Contact* contact, jl::FixtureUserData * thisFixUD, Collidable::Ptr thatCollidable, jl::FixtureUserData * thatFixUD)
{
    // First run through all collisions handled by superclasses.
    Character::endContact(contact, thisFixUD, thatCollidable, thatFixUD);

    // Get the type of the other collidable for comparisons.
    string thatType = thatCollidable->type();

    if(thatType.compare("Platform") == 0
       && thisFixUD->name().compare("Bottom") == 0
       && static_cast<ZPlatform*>(thatCollidable.ptr())->activated())
    {
        _footContacts--;
    }

    if((thatType.compare("MovingPlatform") == 0 || thatType.compare("RectShape") == 0)
       && thisFixUD->name().compare("Bottom") == 0)
    {
        _footContacts--;
    }

    if(thatType.compare("Enemy") == 0
       && thisFixUD->name().compare("Bottom") == 0
       && thatFixUD->name().compare("Head") == 0)
    {
        _footContacts--;
        _damageTaken--;
    }

    if(thatType.compare("GravityField") == 0 && thisFixUD->name().compare("Root"))
    {
        if(_nearbyGravityFields.size() == 0)
        {
            _floatLeft = false;
            _floatRight = false;
        }
    }

    if(thatType.compare("Grapple") == 0 && thisFixUD->name().compare("ZorbitGrappleZone") == 0)
    {
        static_cast<ZGrapple*>(thatCollidable.ptr())->deactivate();
        removeFromGrapList(thatCollidable->name());
    }
    if((thatType.compare("Enemy") == 0 || thatType.compare("Boss") == 0)
       && (thatFixUD->name().compare("Left") == 0 || thatFixUD->name().compare("Right") == 0)
       && thisFixUD->name().compare("Root") == 0)
    {
        _damageTaken--;
    }
}

void Zorbit::handle(std::string input)
{
    if(!_actionMan->peekAction()) return;

    if(!_canMove)
    {
        //actionMan()->clearActions();
        //actionMan()->pushAction(_knownActions["Stop"]);
        return;
    }

    if(input.compare("Right") == 0)
    {
        // So Zorbit will swing right away if he attaches to a grapple.
        _swingRight = true;

        // On the ground
        if(grounded() && _currentAction.compare("MoveRight") != 0
           && !grappling())
           {
                if(_currentAction.compare("MoveLeft") == 0) _actionMan->endAction();
                actionMan()->pushAction(knownAction("MoveRight"));

           }

        // Falling, Jumping, or Bouncing.
        if(!grounded() && !grappling())
        {
            // Rolled off of something
            if(_currentAction.compare("MoveRight") == 0)
            {
                actionMan()->endAction();
                actionMan()->pushAction(knownAction("Fall"), "Top", true);
            }
            if(gravityField())
                _floatRight = true;
            if(actionMan()->peekAction("Next"))
            {
                if(actionMan()->peekAction("Next")->name() != "MoveRight")
                    actionMan()->pushAction(knownAction("MoveRight"), "Next", false);
            }
        }

        // Grappling
        if(grappling())
        {
            _floatRight = true; // So that Zorbit will float right if he disconnects.

            // Rolled off of something
            if(!grounded() && _currentAction.compare("MoveRight") == 0)
            {
                actionMan()->endAction();
                actionMan()->pushAction(knownAction("Fall"), "Top", true);
            }

            if(!grounded() && actionMan()->peekAction("Next")->name() != "MoveRight")
                actionMan()->pushAction(knownAction("MoveRight"), "Next", false);
        }
    }

    if(input.compare("Left") == 0)
    {
        // So Zorbit will swing right away if he attaches to a grapple.
        _swingLeft = true;

        // On the ground
        if(grounded() && _currentAction.compare("MoveLeft") != 0
           && !grappling())
           {
                if(_currentAction.compare("MoveRight") == 0) _actionMan->endAction();
                actionMan()->pushAction(knownAction("MoveLeft"));
           }

        // Falling, Jumping, or Bouncing.
        if(!grounded() && !grappling())
        {
            // Rolled off of something
            if(_currentAction.compare("MoveLeft") == 0)
            {
                actionMan()->endAction();
                actionMan()->pushAction(knownAction("Fall"), "Top", true);
            }
            if(gravityField())
                _floatLeft = true;
            if(actionMan()->peekAction("Next"))
            {
                if(actionMan()->peekAction("Next")->name() != "MoveLeft")
                    actionMan()->pushAction(knownAction("MoveLeft"), "Next", false);
            }
        }

        // Grappling
        if(grappling())
        {
            _floatLeft = true; // So that Zorbit will float right if he disconnects.

            // Rolled off of something
            if(_currentAction.compare("MoveLeft") == 0)
            {
                actionMan()->endAction();
                actionMan()->pushAction(knownAction("Fall"));
            }

            if(!grounded() && actionMan()->peekAction("Next")->name() != "MoveLeft")
                actionMan()->pushAction(knownAction("MoveLeft"), "Next", false);
        }
    }
    if(input.compare("Jump") == 0)
    {
        if(grappling())
        {
            disconnectFromGrapple();
        }
        else if(_currentAction.compare("Jump") != 0 && grounded())
        {
            if(_currentAction.compare("MoveRight") ==0)
                _floatRight = true;
            if(_currentAction.compare("MoveLeft") ==0)
                _floatLeft = true;

            actionMan()->pushAction(knownAction("Jump"));
            level()->soundMan()->playSound("zorbit_jump");
        }
    }
}

void Zorbit::updateConcrete()
{
    if(level()->name().compare("junkyard") == 0)
    {
        if(static_cast<ZMovingPlatform*>(level()->entityMan()->entity("MovingPlatform5").ptr())->moving())
        {
            float pAngle = jl::Tool::sfAngle(polarAngle() + 3*jl::Tool::PI() / 2);
            float screenAngle = 360 - level()->camera()->getView().getRotation();
            if(pAngle > 360) pAngle -= 360;
            if(screenAngle < 0) screenAngle += 360;
            if(pAngle > screenAngle + 70)
                _chronos["junkyard"].resume();
            else _chronos["junkyard"].reset(false);

            if(_chronos["junkyard"].getElapsedTime().asSeconds() > 1)
                _currentHealth = 0;
        }
    }

    // Died
    if(_currentHealth <= 0)
    {
        if(!_dieing)
        {
            _dieing = true;
            changeToDeathTexture();
            level()->camera()->setTarget(this);
            level()->camera()->zoomToOriginal();
            level()->soundMan()->stopAll();
            level()->stopInput();
            //static_cast<ZorbitLevel*>(level())->resetHR();
            _actionMan->pushAction(_knownActions["Die"]);
        }
    }
    else
    {
        if(gravityField()) _chronos["space"] .reset(true);
        else if(_chronos["space"].getElapsedTime().asSeconds() >= 5)
            static_cast<ZorbitLevel*>(level())->bullockedIs(true);

        _chronos["death"].reset(true);

        bossUpdates();

        if(_stunned)
        {
            level()->stopInput();
            blink();
        }
        else unblink();

        if(!_canMove && actionMan()->peekAction()->name().compare("Bounce") != 0)
        {
            stopCompletely();
            return;
        }

        // Still
        if(!grappling())
        {
            if(noActions())
                _actionMan->pushAction(_knownActions["Stop"]);

            // Falling
            if(_footContacts > 0 && _currentAction.compare("Fall") == 0)
                _actionMan->endAction("Fall");
            if(_footContacts <= 0)
            {
                if(_currentAction.compare("Fall") != 0
                && _currentAction.compare("Jump") != 0
                && _currentAction.compare("Bounce") != 0
                && !_grappling)
                {
                    _actionMan->pushAction(knownAction("Fall"));
                }
            }
        }

        // Floating
        if(_floatRight && !grappling())
            floatRight();
        if(_floatLeft && !grappling())
            floatLeft();

        // Swinging on a grapple point
        if(grappling() && belowGrapple() && _swingRight)
            swingRight();
        if(grappling() && belowGrapple() && _swingLeft)
            swingLeft();

        // Attached to an unanchored grapple point
        if(_floatRight && grappling() && !_currentGrapple->anchored())
            floatRight();
        if(_floatLeft && grappling() && !_currentGrapple->anchored())
            floatLeft();

        // Being attacked.
        if(_damageTaken > 0 && _chronos["invincible"].getElapsedTime().asSeconds() > 0.5 && !_invincible)
        {
            if(_currentHealth - _damageTaken < 0)
                _currentHealth = -1;
            else
                hit(_damageTaken);

            _chronos["invincible"].reset(true);
        }
        if(_damageTaken < 0)
            _damageTaken = 0;

        // Getting cogs
        if(_cogsToIncrease > 0)
        {
            _numCogs++;
            _cogsToIncrease--;
            level()->soundMan()->playSound("zorbit_cog_pickup", true);
        }
        if(_cogsToIncrease < 0)
        {
            if(_numCogs > 0)
            {
                _numCogs--;
                _cogsToIncrease++;
            }
            else
            {
                _numCogs = 0;
                _cogsToIncrease = 0;
            }
        }
    }
}

void Zorbit::bossUpdates()
{
    if(level()->name().compare("vegas") == 0)
    {
        if(_footContacts > 0
           && level()->entityMan()->entity("ElvisBoss")
           && static_cast<ZElvisBoss*>(level()->entityMan()->entity("ElvisBoss").ptr())->stunning()
           && !_stunned)
        {
            _actionMan->clearActions();
            _stunned = true;
            _chronos["stun"].reset(true);
        }
        if(_chronos["stun"].getElapsedTime().asSeconds() > 2
            && _stunned)
        {
            _stunned = false;
            level()->resumeInput();
        }
        if (static_cast<ZorbitLevel*>(level())->levelTime().asSeconds() <= 0.1)
        {
            _stunned = false;
            level()->resumeInput();
        }
    }
}

// Movement related
void Zorbit::move(std::string direction)
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
                                                _maxSpeed * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
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
                                                -_maxSpeed * gravityField()->specificForce() / gravityField()->defaultSpecificForce());
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

void Zorbit::hit(int damage)
{
    if(static_cast<ZorbitsOrbits*>(level()->game())->_godMode) return;

    jl::Character::hit();

    if(_chronos["invincible"].getElapsedTime().asMilliseconds() > 30)
    {
        // Stop input and bump Zorbit
        rootBody()->b2body()->SetLinearVelocity(b2Vec2(0,0));
        rootBody()->b2body()->SetAngularVelocity(0);
        rootBody()->b2body()->ApplyLinearImpulse(30 * b2Vec2(-cos(polarAngle()), -sin(polarAngle())),
                                                gravityField()->center());

        // Take damage.
        decreaseHealth(damage);

        // Play sound.
        if(level()->soundMan()->sound("zorbit_hit"))
            level()->soundMan()->playSound("zorbit_hit", true);

        _chronos["invincible"].reset(true);
    }
}

void Zorbit::setPosition(b2Vec2 position)
{
    destroyBodies();

    createBodies(position, 0);
    defineFixtures(_width, _height);
}

void Zorbit::enableWheel()
{
    _wheelJoint->EnableMotor(true);
}

void Zorbit::disableWheel()
{
    _wheelJoint->EnableMotor(false);
}

void Zorbit::bounce()
{
    rootBody()->b2body()->ApplyLinearImpulse(_bounceStrength * _bounceVec, rootBody()->b2body()->GetWorldCenter());
}

void Zorbit::floatUp()
{
    rootBody()->b2body()->ApplyForceToCenter(b2Vec2(1000 * -sin(angle()), 1000 * cos(angle())));
}

void Zorbit::floatDown()
{
    rootBody()->b2body()->ApplyForceToCenter(b2Vec2(1000 * sin(angle()), 1000 * -cos(angle())));
}

void Zorbit::floatRight()
{
    if(!gravityField()) return;
    if(!directedRight())
        switchDirection();
    if(_grappling && _currentGrapple->anchored() && polarVelocity().x < _maxSpeed)
        rootBody()->applyPolarForceToCenter(_currentGrapple->position(), 0, -100);
    else if(polarVelocity().x < std::min(_maxSpeed * 0.75, 20.0))
        rootBody()->applyPolarForceToCenter(0, 100);
}

void Zorbit::floatLeft()
{
    if(!gravityField()) return;
    if(directedRight())
        switchDirection();
    if(_grappling && _currentGrapple->anchored() && polarVelocity().x > -_maxSpeed)
        rootBody()->applyPolarForceToCenter(_currentGrapple->position(), 0, 100);
    else if(polarVelocity().x > -std::min(_maxSpeed * 0.75, 20.0))
        rootBody()->applyPolarForceToCenter(0, -100);
}

void Zorbit::setHealth(int health)
{
    _currentHealth = health;
}

void Zorbit::setMaxSpeed(int speed)
{
    _maxSpeed = speed * _speedMultiplier;

    // Adjust friction of Zorbit's wheel otherwise changing
    // max speed may have no effect.
    body("ZorbitWheel")->b2body()->GetFixtureList()->SetFriction(std::max(speed, 20));
}

// Other actions.
void Zorbit::shoot()
{
    // Restrict shot firing rate to avoid crashing.
    if(_chronos["shot"].getElapsedTime().asMilliseconds() > 100)
    {
        //if(!level()->entityMan()->entity("ZShot1") && _numZShots > MAX_ZSHOTS)
        //   _numZShots = 1;

        // generate shot name
        _shotName.str("");
        _shotName.clear();
        _shotName << "ZShot" << _numZShots;

        // set shot direction
        b2Vec2 gunPos;
        if(directedRight())
        {
            _shotDirection = "Right";
            gunPos = body("ZorbitGunRight")->b2body()->GetPosition();
        }
        else
        {
            _shotDirection = "Left";
            gunPos = body("ZorbitGunLeft")->b2body()->GetPosition();
        }

        if(gravityField())
        {
            float shotRadius = jl::Tool::distance(gunPos, gravityField()->position());
            level()->entityMan()->entityNew(_shotName.str(), "Layer1", new ZShot(_shotName.str(), static_cast<ZorbitLevel *>(level()), _layerNames, gunPos, shotRadius, _shotDirection));
        }
        else
        {
            float angle = 0;
            if(_shotDirection.compare("Right") == 0)
                angle = - this->angle() + (jl::Tool::PI() / 2);
            else
                angle = - this->angle() - (jl::Tool::PI() / 2);
            level()->entityMan()->entityNew(_shotName.str(), "Layer1", new ZShot(_shotName.str(), static_cast<ZorbitLevel *>(level()), _layerNames, gunPos, angle));
        }

        _numZShots++;
        level()->soundMan()->playSound("zorbit_shoot", true);
        _chronos["shot"].reset(true);
    }
}

void Zorbit::stop()
{
    _wheelJoint->SetMotorSpeed(0);
    disableWheel();
}

void Zorbit::stopCompletely()
{
    stop();
    actionMan()->clearActions();
    if(!grappling()) actionMan()->pushAction(_knownActions["Stop"]);
    else actionMan()->pushAction(_knownActions["Grapple"]);
    _canMove = false;
    _floatLeft = false;
    _floatRight = false;
}

void Zorbit::unStop()
{
    actionMan()->clearActions();
    if(!grappling()) actionMan()->pushAction(_knownActions["Stop"]);
    else actionMan()->pushAction(_knownActions["Grapple"]);
    _canMove = true;
    enableWheel();
}

void Zorbit::die()
{
    if(_chronos["death"].getElapsedTime().asSeconds() > 2)
    {
        _dieing = false;

        if(!static_cast<ZorbitsOrbits*>(level()->game())->loseLife())
        {
            static_cast<ZorbitsOrbits*>(level()->game())->gameIsOver(true);
            return;
        }

        // Music resets.
        if(level()->name().compare("tutorial") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint5") == 0)
           level()->soundMan()->currentMusicIs("robotropolis");

        if(level()->name().compare("highway") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint5") == 0)
           level()->soundMan()->currentMusicIs("vex");

        if(level()->name().compare("wondercog") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint4") == 0)
           level()->soundMan()->currentMusicIs("wondercog");

        if(level()->name().compare("vegas") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint2") == 0)
           level()->soundMan()->currentMusicIs("vegas");

        if(level()->name().compare("junkyard") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint5") == 0)
           level()->soundMan()->currentMusicIs("junkyard");

        if(level()->name().compare("jungle") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint5") == 0)
           level()->soundMan()->currentMusicIs("jungle");

        // Custom object resets.
        if(level()->name().compare("vegas") == 0)
        {
            std::list<std::string> doNotReset;
            doNotReset.push_back("button2");
            doNotReset.push_back("button4");
            doNotReset.push_back("Platform1");
            doNotReset.push_back("Platform2");
            doNotReset.push_back("Platform3");
            doNotReset.push_back("Platform4");
            doNotReset.push_back("Platform5");
            level()->resetExcept(doNotReset);
        }
        else if(level()->name().compare("junkyard") == 0)
        {
            std::list<std::string> doNotReset;
            doNotReset.push_back("MovingPlatform6");
            doNotReset.push_back("Platform1");
            level()->resetExcept(doNotReset);
        }
        else level()->reset();

        actionMan()->pushAction(_knownActions["Stop"]);
        level()->resumeInput();

        // Zooms resets.
        if(level()->name().compare("wondercog") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint4") == 0)
           {
              level()->camera()->zoom(1.75);
              setMaxSpeed(50);
           }

        if(level()->name().compare("vegas") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint1") == 0)
            level()->camera()->zoom(1.5);

        if(level()->name().compare("jungle") == 0
           && (static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint1") == 0
               || static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint2") == 0))
            level()->camera()->zoom(1.5);

        /*if(level()->name().compare("junkyard") == 0
           && static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint6") == 0)
            level()->camera()->zoom(1.8);
        */
        if(level()->name().compare("jungle") == 0
           && (static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint4") == 0
               || static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint3") == 0
               || static_cast<ZorbitLevel*>(level())->checkPoint().compare("checkpoint5") == 0))
            level()->camera()->zoom(1.8);
    }
}

void Zorbit::blink()
{
    rootBody()->sprite()->setTextureRect(sf::IntRect(300,0,150,150));

    if(_chronos["blink"].getElapsedTime().asMilliseconds() < 100)
    {
        std::map<std::string, jl::Body::Ptr>::iterator itr;
        for(itr = bodyItrBegin(); itr != bodyItrEnd(); itr++)
        {
            itr->second->sprite()->setColor(sf::Color::Blue);
        }
    }
    else if(_chronos["blink"].getElapsedTime().asMilliseconds() < 200)
    {
        std::map<std::string, jl::Body::Ptr>::iterator itr;
        for(itr = bodyItrBegin(); itr != bodyItrEnd(); itr++)
        {
            itr->second->sprite()->setColor(sf::Color(255,255,255,255));
        }
    }
    else
        _chronos["blink"].reset(true);
}

void Zorbit::unblink()
{
    std::map<std::string, jl::Body::Ptr>::iterator itr;
    for(itr = bodyItrBegin(); itr != bodyItrEnd(); itr++)
    {
        itr->second->sprite()->setColor(sf::Color(255,255,255,255));
    }
}

// ----- Grapple
void Zorbit::createGrappleChain()
{
    _grappleChain = new GrappleChain(this, level(), _layerNames, position());
    level()->entityMan()->entityNew(_grappleChain->name(), "Layer1", _grappleChain);
}

ZGrapple::Ptr Zorbit::closestGrapple()
{
    ZGrapple::Ptr closest;

    if(!_grapPoints.empty())
        closest = (*_grapPoints.begin());
    else
        return NULL;

    std::list<ZGrapple::Ptr>::iterator itr;
    for(itr = _grapPoints.begin(); itr != _grapPoints.end(); itr++)
    {
        if(jl::Tool::distance(position(), (*itr)->position()) < jl::Tool::distance(position(), closest->position()))
            closest = (*itr);
    }

    return closest;
}

void Zorbit::removeFromGrapList(std::string name)
{
   std::list<ZGrapple::Ptr>::iterator itr;
    for(itr = _grapPoints.begin(); itr != _grapPoints.end(); itr++)
    {
        if((*itr)->name().compare(name) == 0)
            itr = _grapPoints.erase(itr);
    }
}

void Zorbit::connectToGrapple()
{
    _currentGrapple = closestGrapple().ptr();
    if(_currentGrapple->hasZCircle())
        _currentGrapple->zCircle()->disableLimits();

    // Set up rope joint fixture stuff
    b2RopeJointDef ropeJointDef;
    ropeJointDef.bodyA = rootBody()->b2body();
    ropeJointDef.bodyB = _currentGrapple->rootBody()->b2body();
    ropeJointDef.localAnchorA = b2Vec2(0,0);
    ropeJointDef.localAnchorB = b2Vec2(0,0);
    ropeJointDef.maxLength = GRAPPLE_RADIUS;
    ropeJointDef.collideConnected = true;

    _grappleJoint = (b2RopeJoint *)level()->b2world()->CreateJoint(&ropeJointDef);
    _grappleChain->attach(closestGrapple().ptr());
    _grappling  = true;

    actionMan()->pushAction(knownAction("Grapple"));
    level()->soundMan()->playSound("grap_connect");
}

void Zorbit::disconnectFromGrapple()
{
    level()->b2world()->DestroyJoint(_grappleJoint);
    _grappleJoint = NULL;
    _currentGrapple = NULL;
    _grappleChain->detach();
    _grappling  = false;
    actionMan()->endAction("Grapple");
    actionMan()->pushAction(knownAction("Fall"));
}

bool Zorbit::belowGrapple() const
{
    if(!gravityField()) return false;
    if(!_currentGrapple) return false;
    if(!_currentGrapple->anchored()) return false;
    return (abs(jl::Tool::distance(position(), gravityField()->position()))
            < abs(jl::Tool::distance(_currentGrapple->position(), gravityField()->position())));
}

void Zorbit::swingLeft()
{
    if(_currentGrapple)
    {
        if(directedRight())
            switchDirection();

        float F;
        if(gravityField()) F = gravityField()->specificForce() * 6.5 / 30;  //TODO instead of 30, get the "Default gravity force"
        else F = 6.5;

        b2Vec2 grapPos = _currentGrapple->position();
        float theta = jl::Tool::polarAngle(position(), grapPos);
        float forceX =  F * sin(theta);
        float forceY =  - F * cos(theta);
        rootBody()->b2body()->ApplyForceToCenter(b2Vec2(F * forceX, F * forceY));
    }
}

void Zorbit::swingRight()
{
    if(_currentGrapple)
    {
        if(!directedRight())
            switchDirection();

        float F;
        if(gravityField()) F = gravityField()->specificForce() * 6.5 / 30;  //TODO instead of 30, get the "Default gravity force"
        else F = 6.5;

        b2Vec2 grapPos = _currentGrapple->position();
        float theta = jl::Tool::polarAngle(position(), grapPos);
        float forceX =  - F * sin(theta);
        float forceY =  F * cos(theta);
        rootBody()->b2body()->ApplyForceToCenter(b2Vec2(F * forceX, F * forceY));
    }
}

void Zorbit::changeGrappleLength(float input)
{
    //TODO make it not jumpy.
    _grappleJoint->SetMaxLength(_grappleJoint->GetMaxLength() + input);
}

void Zorbit::printGrapList()
{
    if(_grapPoints.empty())
        std::cout << "Grapple list is empty!" << std::endl;
    else
    {
        std::list<ZGrapple::Ptr>::iterator itr;
        for(itr = _grapPoints.begin(); itr != _grapPoints.end(); itr++)
        {
            std::cout << (*itr)->name() << std::endl;
        }
    }
}

void Zorbit::changeToDeathTexture()
{
    if(_vex) return;
    rootBody()->changeTexture(level()->game()->textureMan()->textureDesc("resources/zorbitsorbits/zorbit/zorbitdeath.png"));
}

void Zorbit::changeToOriginalTexture()
{
    if(_vex) return;
    rootBody()->changeTexture(level()->game()->textureMan()->textureDesc("resources/zorbitsorbits/zorbit/torso.png"));
}

void Zorbit::cleanUp()
{
    _grapPoints.clear();

    Character::cleanUp();
}

